#include <LoadPalette.h>
#include <PaletteImage.h>

#include <molecular/util/CommandLineParser.h>
#include <molecular/util/FileStreamStorage.h>

#include <iostream>

using namespace molecular::util;

std::vector<uint8_t> GenerateColormap(const uint8_t palette[768])
{
	std::vector<uint8_t> colormap(16384);
	for(int x = 0; x < 256; x++)
	{
		for(int y = 0; y < 64; y++)
		{
			if(x >= 224)
			{
				colormap[y * 256 + x] = x;
			}
			else
			{
				uint8_t rgb[3];

				for (int i = 0; i < 3; i++)
					rgb[i] = std::min((palette[x * 3 + i] * (63 - y) + 16) >> 5, 256);

				colormap[y * 256 + x] = FindClosestPaletteColor(rgb, palette, 224);
			}
		}
	}
	return colormap;
}

int Main(int argc, char** argv)
{
	CommandLineParser cmd;
	CommandLineParser::PositionalArg<std::string> inFileName(cmd, "input file", "Input palette file");
	CommandLineParser::PositionalArg<std::string> outFileName(cmd, "output file", "Output colormap file");
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

	auto palette = LoadPaletteFile(inFileName->c_str());
	auto colormap = GenerateColormap(palette.data());

	FileWriteStorage outFile(outFileName->c_str());
	outFile.Write(colormap.data(), 16384);
	const uint8_t terminator = 0x20;
	outFile.Write(&terminator, 1);

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
