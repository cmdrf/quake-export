/*	MdlExportMain.cpp

MIT License

Copyright (c) 2023 Fabian Herb

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <LoadPalette.h>
#include "MdlFile.h"
#include "MdlJson.h"
#include "MdlUtils.h"
#include <TextureImage.h>
#include <QuakePalette.h>
#include <StbImage.h>

#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/ObjFile.h>
#include <molecular/util/ObjFileUtils.h>
#include <molecular/util/StringUtils.h>
#include <molecular/util/CommandLineParser.h>

#include <fstream>
#include <iostream>

using namespace molecular;
using namespace molecular::util;

void WriteSimpleMdl(
		const std::vector<uint32_t>& indices,
		const std::vector<Vector3>& positions,
		const std::vector<Vector3>& normals,
		const std::vector<Vector2>& uvs,
		const std::vector<uint8_t>& skin,
		int skinWidth, int skinHeight,
		const std::string& outFile)
{
	assert(positions.size() == normals.size());
	assert(positions.size() == uvs.size());
	assert(skin.size() == skinWidth * skinHeight);

	auto [min, max] = GetMinMax(positions);
	FileWriteStorage file(outFile);
	MdlFile mdl(file);
	MdlFile::Header header;
	header.scale = (max - min) / 255.0;
	header.origin = min;
	header.radius = CalculateBoundingRadius(min, max);
	header.numSkins = 1;
	header.skinWidth = skinWidth;
	header.skinHeight = skinHeight;
	header.numVerts = positions.size();
	header.numTris = indices.size() / 3;
	header.numFrames = 1;
	header.size = CalculateAverageTriangleArea(indices, positions);

	mdl.WriteHeader(header);
	mdl.WriteSkin(skin.data());
	for(auto& uv: uvs)
		mdl.WriteStVertex(false, uv[0] * skinWidth, uv[1] * skinHeight);
	for(size_t i = 0; i < indices.size(); i += 3)
		mdl.WriteTriangle(true, indices.data() + i);
	MdlFile::SimpleFrame frame;
	frame.vertices = ToTriangleVertices(positions, normals, header.origin, header.scale);
	auto [minV, maxV] = GetMinMax(frame.vertices);
	frame.min = minV;
	frame.max = maxV;
	frame.name = "frame1";
	mdl.WriteSingleFrame(frame);
}

void ProcessStaticModel(const std::string& objPath, const std::string& outputPath, const std::string& texturePath, const uint8_t* paletteData, bool dither)
{
	TextureImage textureImage(texturePath.c_str());
	auto skin = textureImage.ToIndexed(paletteData, dither);

	std::vector<uint32_t> indices;
	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> uvs;

	ReadObj(objPath, indices, positions, normals, uvs);
	WriteSimpleMdl(indices, positions, normals, uvs, skin, textureImage.GetWidth(), textureImage.GetHeight(), outputPath);
}


void ProcessComplexModel(const std::string& jsonPath, const std::string& outputPath, const uint8_t* paletteData, bool dither)
{
	MdlJson::Data data = MdlJson::Read(jsonPath);

	auto [skinWidth, skinHeight] = data.GetSkinWidthHeight();

	std::vector<Vector3> allPositions = data.CollectAllPositions();
	auto [min, max] = GetMinMax(allPositions);

	MdlFile::Header header;
	header.scale = (max - min) / 255.0;
	header.origin = min;
	header.radius = CalculateBoundingRadius(min, max);
	header.numSkins = data.skins.size();
	header.skinWidth = skinWidth;
	header.skinHeight = skinHeight;
	header.numVerts = data.mainPositions.size();
	header.numTris = data.mainIndices.size() / 3;
	header.numFrames = data.frames.size();
	header.size = CalculateAverageTriangleArea(data.mainIndices, data.mainPositions);

	FileWriteStorage file(outputPath);
	MdlFile mdl(file);
	mdl.WriteHeader(header);

	// Write skins:
	for(auto& skin: data.skins)
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, MdlJson::SimpleSkin>)
			{
				auto indexedSkin = arg.ToIndexed(paletteData, dither);
				mdl.WriteSkin(indexedSkin.data());
			}
			else if constexpr (std::is_same_v<T, MdlJson::SkinGroup>)
			{
				std::vector<std::vector<uint8_t>> skins;
				for(auto& skin: arg.skins)
				{
					skins.push_back(skin.ToIndexed(paletteData, dither));
				}
				mdl.WriteSkinGroup(arg.times, skins);
			}
		}, skin);

	// Write UVs and triangles:
	for(auto& uv: data.mainUvs)
		mdl.WriteStVertex(false, uv[0] * skinWidth, uv[1] * skinHeight);
	for(size_t i = 0; i < data.mainIndices.size(); i += 3)
		mdl.WriteTriangle(true, data.mainIndices.data() + i);

	// Write frames:
	for(auto& frame: data.frames)
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, MdlJson::SimpleFrame>)
			{
				if(arg.positions.size() != data.mainPositions.size())
				{
					std::ostringstream oss;
					oss << "Vertex count varies between frames. " << arg.positions.size() << " vs. " << data.mainPositions.size();
					throw std::runtime_error(oss.str());
				}
				MdlFile::SimpleFrame frame;
				frame.vertices = ToTriangleVertices(arg.positions, arg.normals, header.origin, header.scale);
				auto [minV, maxV] = GetMinMax(frame.vertices);
				frame.min = minV;
				frame.max = maxV;
				frame.name = arg.name;
				mdl.WriteSingleFrame(frame);
			}
			else if constexpr (std::is_same_v<T, MdlJson::FrameGroup>)
			{
				std::vector<MdlFile::SimpleFrame> frames;
				for(auto& frame: arg.frames)
				{
					if(frame.positions.size() != data.mainPositions.size())
						throw std::runtime_error("Vertex count varies between frames");
					MdlFile::SimpleFrame mdlFrame;
					mdlFrame.vertices = ToTriangleVertices(frame.positions, frame.normals, header.origin, header.scale);
					auto [minV, maxV] = GetMinMax(mdlFrame.vertices);
					mdlFrame.min = minV;
					mdlFrame.max = maxV;
					mdlFrame.name = "frame1";
					frames.push_back(mdlFrame);
				}

				// Get overall min/max:
				MdlFile::TriangleVertex max = {{0, 0, 0}, 0};
				MdlFile::TriangleVertex min = {{255, 255, 255}, 0};
				for(auto& frame: frames)
				{
					for(int i = 0; i < 3; ++i)
					{
						min.packedPositions[i] = std::min(min.packedPositions[i], frame.min.packedPositions[i]);
						max.packedPositions[i] = std::max(max.packedPositions[i], frame.max.packedPositions[i]);
					}
				}
				mdl.WriteFrameGroup(min, max, arg.times, frames);
			}
		}, frame);
}

int Main(int argc, char** argv)
{
	CommandLineParser cmd;
	CommandLineParser::PositionalArg<std::string> inFileName(cmd, "input file", "Input mesh");
	CommandLineParser::PositionalArg<std::string> outFileName(cmd, "output file", "Output MDL file");
	CommandLineParser::Flag dither(cmd, "dither", "Enable dithering for textures");
	CommandLineParser::Option<std::string> texture(cmd, "texture", "Texture to use", "");
	CommandLineParser::Option<std::string> palette(cmd, "palette", "Palette to use instead of default Quake palette. Can be image or lump.");
	CommandLineParser::HelpFlag help(cmd);

	try
	{
		cmd.Parse(argc, argv);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		cmd.PrintHelp();
		return EXIT_FAILURE;
	}

	const uint8_t* paletteData = quakePalette;
	std::vector<uint8_t> loadedPalette;
	if(palette)
	{
		loadedPalette = LoadPaletteFile(palette->c_str());
		paletteData = loadedPalette.data();
	}

	if(StringUtils::EndsWith(*inFileName, ".obj"))
	{
		if(!texture)
		{
			std::cerr << "Need to set texture for single mesh\n";
			return EXIT_FAILURE;
		}

		ProcessStaticModel(*inFileName, *outFileName, *texture, paletteData, dither);
	}
	else if(StringUtils::EndsWith(*inFileName, ".json"))
	{
		ProcessComplexModel(*inFileName, *outFileName, paletteData, dither);
	}
	else
		throw std::runtime_error("Unrecognized input file type");
	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
	try
	{
		return Main(argc, argv);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
