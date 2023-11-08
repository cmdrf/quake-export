#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/StringUtils.h>
#include <molecular/util/CommandLineParser.h>

using namespace molecular;
using namespace molecular::util;

struct PakHeader
{
	char magic[4] = {'P', 'A', 'C', 'K'};
	uint32_t diroffset = 0;
	uint32_t dirsize = 0;
};

struct PakEntry
{
	char filename[56];
	uint32_t offset;
	uint32_t size;
};

int Main(int argc, char** argv)
{
	CommandLineParser cmd;
	CommandLineParser::PositionalArg<std::string> outFileName(cmd, "output file", "Output PAK file");
	CommandLineParser::RemainingPositionalArgs remaining(cmd, "input files", "Input files");
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

	PakHeader header;
	outFile.Write(&header, sizeof(PakHeader));
	std::vector<PakEntry> entries;

	for(auto& fileName: *remaining)
	{
		FileReadStorage file(fileName);
		size_t size = file.GetSize();
		std::vector<uint8_t> fileData(size);
		file.Read(fileData.data(), size);

		PakEntry entry;
		memset(entry.filename, 0, 56);
		strncpy(entry.filename, fileName.c_str(), 56);
		entry.size = size;
		entry.offset = outFile.GetCursor();
		entries.push_back(entry);
		outFile.Write(fileData.data(), fileData.size());
	}

	header.diroffset = outFile.GetCursor();
	header.dirsize = entries.size() * sizeof(PakEntry);

	outFile.Write(entries.data(), entries.size() * sizeof(PakEntry));
	outFile.SetCursor(0);
	outFile.Write(&header, sizeof(PakHeader));

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
