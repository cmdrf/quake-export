#include <LoadPalette.h>
#include <StbImage.h>
#include <PaletteImage.h>
#include <QuakePalette.h>

#include <molecular/util/CommandLineParser.h>
#include <molecular/util/FileStreamStorage.h>

#include <iostream>

using namespace molecular;
using namespace molecular::util;

int Main(int argc, char** argv)
{
	CommandLineParser cmd;
	CommandLineParser::PositionalArg<std::string> inFileName(cmd, "input file", "Input image");
	CommandLineParser::PositionalArg<std::string> outFileName(cmd, "output file", "Output .lmp file");
	CommandLineParser::Flag dither(cmd, "dither", "Enable dithering");
	CommandLineParser::Option<std::string> palette(cmd, "palette", "Palette to use instead of default Quake palette. Can be image or lump.");
	CommandLineParser::HelpFlag help(cmd);

	cmd.Parse(argc, argv);

	const uint8_t* paletteData = quakePalette;
	std::vector<uint8_t> loadedPalette;
	if(palette)
	{
		loadedPalette = LoadPaletteFile(palette->c_str());
		paletteData = loadedPalette.data();
	}

	StbImage textureImage(inFileName->c_str(), 3);
	const int32_t width = textureImage.GetWidth();
	const int32_t height = textureImage.GetHeight();

	std::vector<uint8_t> indexedImage = ConvertToIndexed(textureImage.Data(), width, height, paletteData, dither);

	FileWriteStorage outFile(outFileName->c_str());
	outFile.Write(&width, 4);
	outFile.Write(&height, 4);
	outFile.Write(indexedImage.data(), indexedImage.size());

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
