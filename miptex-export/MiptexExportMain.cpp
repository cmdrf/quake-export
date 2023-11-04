#include "MiptexFile.h"

#include <LoadPalette.h>
#include <PaletteImage.h>
#include <QuakePalette.h>
#include <TextureImage.h>
#include <Transparency.h>
#include <WriteImage.h>

#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/StringUtils.h>
#include <molecular/util/CommandLineParser.h>

using namespace molecular;
using namespace molecular::util;

int Main(int argc, char** argv)
{
	CommandLineParser cmd;
	CommandLineParser::PositionalArg<std::string> inFileName(cmd, "input file", "Input image");
	CommandLineParser::PositionalArg<std::string> outFileName(cmd, "output file", "Output MIPTEX file");
	CommandLineParser::Flag dither(cmd, "dither", "Enable dithering");
	CommandLineParser::Option<std::string> palette(cmd, "palette", "Palette to use instead of default Quake palette. Can be image or lump.");
	CommandLineParser::Option<float> hdrScale(cmd, "hdr-scale", "Controls brightness when using HDR images.", 1.0f);
	CommandLineParser::Option<std::string> previewOutput(cmd, "preview-output", "Write quantized image back to file");
	CommandLineParser::Option<std::string> nameOption(cmd, "name", "Name of the texture embedded in file. Defaults to file name without extension.");
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

	FileWriteStorage outFile(outFileName->c_str());
	TextureImage textureImage(inFileName->c_str());
	const auto width = textureImage.GetWidth();
	const auto height = textureImage.GetHeight();

	if(width % 8 != 0 || height % 8 != 0)
	{
		std::cerr << "Width and height must be multiples of 8, preferrably powers of two" << std::endl;
		return EXIT_FAILURE;
	}

	MiptexFile outMiptexFile(outFile);
	const std::string name = nameOption ? *nameOption : StringUtils::FileNameWithoutExtension(*inFileName);
	outMiptexFile.WriteHeader(name.c_str(), width, height);

	for(int i = 0; i < 4; ++i)
	{
		auto indexedImage = textureImage.ToIndexed(paletteData, dither, i, *hdrScale);
		outMiptexFile.WriteMip(indexedImage.data(), indexedImage.size());

		if(i == 0 && previewOutput)
		{
			auto previewImage = ConvertToRgb(indexedImage.data(), width, height, paletteData);
			WriteRgbImage(previewOutput->c_str(), previewImage.data(), width, height);
		}
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
