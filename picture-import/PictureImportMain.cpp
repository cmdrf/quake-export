#include <WriteImage.h>
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
	CommandLineParser::PositionalArg<std::string> inFileName(cmd, "input file", "Input .lmp file");
	CommandLineParser::PositionalArg<std::string> outFileName(cmd, "output file", "Output image file. Format is guessed from file extension.");
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

	int32_t width = 0;
	int32_t height = 0;
	std::vector<uint8_t> indexedImage;

	FileReadStorage inFile(inFileName->c_str());
	inFile.Read(&width, 4);
	inFile.Read(&height, 4);
	if(width > 4000 || height > 4000)
		throw std::runtime_error("Unreasonable picture size. Is this really a picture lump?");
	indexedImage.resize(width * height);
	inFile.Read(indexedImage.data(), indexedImage.size());
	auto outImage = ConvertToRgb(indexedImage.data(), width, height, paletteData);
	WriteRgbImage(outFileName->c_str(), outImage.data(), width, height);

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
