#include "WadFile.h"

#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/StringUtils.h>
#include <molecular/util/CommandLineParser.h>

using namespace molecular;
using namespace molecular::util;

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
		wadFile.AddFile(name.c_str(), WadFile::MIPTEX, fileData.data(), size);
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
