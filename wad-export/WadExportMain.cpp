#include "WadFile.h"

#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/StringUtils.h>
#include <molecular/util/CommandLineParser.h>

using namespace molecular;
using namespace molecular::util;

WadFile::FileType IdentifyFile(const std::vector<uint8_t>& data)
{
	if(data.size() < 8)
		throw std::runtime_error("File too short");
	{
		uint32_t width = *reinterpret_cast<const uint32_t*>(data.data());
		uint32_t height = *reinterpret_cast<const uint32_t*>(data.data() + 4);
		if(width * height + 8 == data.size())
			return WadFile::STATUSBAR_PIC;
	}
	if(data.size() == 128*128 || data.size() == 320*200)
		return WadFile::CONSOLE_PIC;
	if(data.size() >= 24)
	{
		uint32_t width = *reinterpret_cast<const uint32_t*>(data.data() + 16);
		uint32_t height = *reinterpret_cast<const uint32_t*>(data.data() + 20);
		size_t expected = 40 + width*height + width*height/4 + width*height/16 + width*height/64;
		if(data.size() == expected)
			return WadFile::MIPTEX;
	}
	if(data.size() == 256*3)
		return WadFile::COLOR_PALETTE;
	throw std::runtime_error("Unrecognized file");
}

int Main(int argc, char** argv)
{
	CommandLineParser cmd;
	CommandLineParser::PositionalArg<std::string> outFileName(cmd, "output file", "Output WAD file");
	CommandLineParser::RemainingPositionalArgs remaining(cmd, "input files", "Input Files");
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
	WadFile wadFile(outFile);

	for(auto& fileName: *remaining)
	{
		auto name = StringUtils::FileNameWithoutExtension(fileName);
		FileReadStorage file(fileName);
		size_t size = file.GetSize();
		std::vector<uint8_t> fileData(size);
		file.Read(fileData.data(), size);
		wadFile.AddFile(name.c_str(), IdentifyFile(fileData), fileData.data(), size);
	}

	wadFile.Finish();

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
