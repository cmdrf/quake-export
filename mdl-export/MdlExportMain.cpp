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

#include "MdlFile.h"
#include <PaletteImage.h>
#include <QuakePalette.h>
#include "QuakeNormal.h"
#include <StbImage.h>

#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/ObjFile.h>
#include <molecular/util/ObjFileUtils.h>
#include <molecular/util/StringUtils.h>
#include <molecular/util/CommandLineParser.h>


#include <iostream>

using namespace molecular;
using namespace molecular::util;

std::pair<Vector3, Vector3> GetMinMax(const std::vector<Vector3>& positions)
{
	Vector3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vector3 max(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
	for(auto& pos: positions)
	{
		for(int i = 0; i < 3; ++i)
		{
			min[i] = std::min(min[i], pos[i]);
			max[i] = std::max(max[i], pos[i]);
		}
	}
	return std::make_pair(min, max);
}

std::pair<MdlFile::TriangleVertex, MdlFile::TriangleVertex> GetMinMax(const std::vector<MdlFile::TriangleVertex>& vertices)
{
	MdlFile::TriangleVertex min = {{0, 0, 0}, 0};
	MdlFile::TriangleVertex max = {{255, 255, 255}, 0};
	for(auto& pos: vertices)
	{
		for(int i = 0; i < 3; ++i)
		{
			min.packedPositions[i] = std::min(min.packedPositions[i], pos.packedPositions[i]);
			max.packedPositions[i] = std::max(max.packedPositions[i], pos.packedPositions[i]);
		}
	}
	return std::make_pair(min, max);
}

float CalculateBoundingRadius(const Vector3& min, const Vector3& max)
{
	Vector3 rsqr;
	for (size_t i = 0; i < 3; ++i)
		rsqr[i] = std::pow(std::max(std::abs(min[i]), std::abs(max[i])), 2);

	// Calculate bounding radius:
	return std::sqrt(rsqr[0] + rsqr[1] + rsqr[2]);
}

float CalculateAverageTriangleArea(
		const std::vector<uint32_t>& indices,
		const std::vector<Vector3>& positions)
{
	float totalArea = 0.0f;

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		Vector3 A = positions[indices[i]];
		Vector3 B = positions[indices[i + 1]];
		Vector3 C = positions[indices[i + 2]];

		Vector3 AB = B - A;
		Vector3 AC = C - A;

		float area = 0.5f * AB.CrossProduct(AC).Length();
		totalArea += area;
	}

	const size_t numTriangles = indices.size() / 3;
	return totalArea / numTriangles;
}

std::vector<MdlFile::TriangleVertex> ToTriangleVertices(const std::vector<Vector3>& positions, const std::vector<Vector3>& normals, const Vector3& origin, const Vector3& scale)
{
	assert(positions.size() == normals.size());

	std::vector<MdlFile::TriangleVertex> out;

	for(size_t i = 0; i < positions.size(); ++i)
	{
		MdlFile::TriangleVertex vert;
		Vector3 pos = (positions[i] - origin) / scale;
		vert.packedPositions[0] = pos[0];
		vert.packedPositions[1] = pos[1];
		vert.packedPositions[2] = pos[2];
		vert.lightNormalIndex = QuakeNormal(normals[i]);
		out.push_back(vert);
	}

	return out;
}

void WriteMdl(
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

int Main(int argc, char** argv)
{
	CommandLineParser cmd;
	CommandLineParser::PositionalArg<std::string> inFileName(cmd, "input file", "Input mesh");
	CommandLineParser::PositionalArg<std::string> outFileName(cmd, "output file", "Output MDL file");
	CommandLineParser::Flag dither(cmd, "dither", "Enable dithering for textures");
	CommandLineParser::Option<std::string> texture(cmd, "texture", "Texture to use (required)", "");
	CommandLineParser::Option<std::string> palette(cmd, "palette", "Palette to use instead of default Quake palette", "");
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

	if(!texture)
	{
		std::cerr << "Need to set texture\n";
		cmd.PrintHelp();
		return EXIT_FAILURE;
	}

	const uint8_t* paletteData = quakePalette;
	std::vector<uint8_t> externalPaletteData;
	if(palette)
	{
		FileReadStorage paletteFile(*palette);
		size_t paletteSize = paletteFile.GetSize();
		if(paletteSize != 768)
		{
			std::cerr << "Not a valid palette file\n";
			return EXIT_FAILURE;
		}
		externalPaletteData.resize(768);
		paletteFile.Read(externalPaletteData.data(), 768);
		paletteData = externalPaletteData.data();
	}

	StbImage textureImage(texture->c_str(), 3);

	auto skin = ConvertToIndexed(textureImage.Data(), textureImage.GetWidth(), textureImage.GetHeight(), paletteData, dither);

	if(StringUtils::EndsWith(*inFileName, ".obj"))
	{
		FileReadStorage inFile(*inFileName);
		TextReadStream<FileReadStorage> trs(inFile);
		ObjFile objFile(trs);

		std::vector<uint32_t> indices;
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector2> uvs;

		for(auto& vg: objFile.GetVertexGroups())
		{
			if(vg.numQuads == 0 && vg.numTriangles == 0)
				continue;

			ObjFileUtils::ObjVertexGroupBuffers(objFile, vg, indices, positions, normals, uvs);
			size_t numVertices = positions.size();
			assert(normals.empty() || normals.size() == numVertices);
			assert(uvs.empty() || uvs.size() == numVertices);
		}

		// Convert to quake units:
		for(auto& pos: positions)
			pos *= 64.0 / 1.7;

		WriteMdl(indices, positions, normals, uvs, skin, textureImage.GetWidth(), textureImage.GetHeight(), *outFileName);
	}
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
