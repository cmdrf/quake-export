#include "MiptexFile.h"

#include <StbImage.h>
#include <PaletteImage.h>
#include <QuakePalette.h>

#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/StringUtils.h>
#include <molecular/util/CommandLineParser.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

using namespace molecular;
using namespace molecular::util;

std::string FileName(const std::string& path)
{
	auto lastSlash = path.find_last_of("/\\");
	if(lastSlash == std::string::npos)
		return path;
	else
		return path.substr(lastSlash + 1);
}

std::string FileNameWithoutExtension(const std::string& path)
{
	auto filename = FileName(path);
	auto firstDot = filename.find_first_of('.');
	if(firstDot == std::string::npos)
		return filename;
	else
		return filename.substr(0, firstDot);
}

std::pair<std::vector<uint8_t>, std::vector<uint8_t>> SplitColorAndAlpha(const uint8_t* data, int width, int height)
{
	std::pair<std::vector<uint8_t>, std::vector<uint8_t>> out;
	out.first.resize(width * height * 3);
	out.second.resize(width * height);
	for(int i = 0; i < width * height; ++i)
	{
		out.first[i * 3] = data[i * 4];
		out.first[i * 3 + 1] = data[i * 4 + 1];
		out.first[i * 3 + 2] = data[i * 4 + 2];
		out.second[i] = data[i * 4 + 3];
	}
	return out;
}

void SetTransparency(std::vector<uint8_t>& indexedImage, const std::vector<uint8_t>& alpha)
{
	assert(indexedImage.size() == alpha.size());

	for(size_t i = 0; i < indexedImage.size(); ++i)
	{
		if(alpha[i] < 128)
			indexedImage[i] = 255;
	}
}

int Main(int argc, char** argv)
{
	CommandLineParser cmd;
	CommandLineParser::PositionalArg<std::string> inFileName(cmd, "input file", "Input image");
	CommandLineParser::PositionalArg<std::string> outFileName(cmd, "output file", "Output MIPTEX file");
	CommandLineParser::Flag dither(cmd, "dither", "Enable dithering");
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

	FileWriteStorage outFile(outFileName->c_str());
	MiptexFile outMiptexFile(outFile);
	StbImage textureImage(inFileName->c_str(), 4);
	const auto width = textureImage.GetWidth();
	const auto height = textureImage.GetHeight();

	if(width % 8 != 0 || height % 8 != 0)
	{
		std::cerr << "Width and height must be multiples of 8, preferrably powers of two" << std::endl;
		return EXIT_FAILURE;
	}

	auto [color, alpha] = SplitColorAndAlpha(textureImage.Data(), width, height);

	std::vector<uint8_t> indexedImage = ConvertToIndexed(color.data(), width, height, quakePalette, dither);
	SetTransparency(indexedImage, alpha);
	outMiptexFile.WriteHeader(FileNameWithoutExtension(*inFileName).c_str(), width, height);
	outMiptexFile.WriteMip(indexedImage.data(), indexedImage.size());

	int newWidth = width / 2;
	int newHeight = height / 2;
	for(int i = 0; i < 3; ++i)
	{
		std::vector<uint8_t> newImage(newWidth * newHeight * 4);
		stbir_resize_uint8_srgb_edgemode(textureImage.Data(), width, height, width * 4, newImage.data(), newWidth, newHeight, newWidth * 4, 4, 3, 0, STBIR_EDGE_WRAP);
		auto [color, alpha] = SplitColorAndAlpha(newImage.data(), newWidth, newHeight);

		indexedImage = ConvertToIndexed(color.data(), newWidth, newHeight, quakePalette, dither);
		SetTransparency(indexedImage, alpha);
		outMiptexFile.WriteMip(indexedImage.data(), indexedImage.size());

		newWidth /= 2;
		newHeight /= 2;
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
