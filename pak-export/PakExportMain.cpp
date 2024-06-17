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
	CommandLineParser::Flag mergePaks(cmd, "merge-paks", "Input files are PAK files to be merged into one");
	CommandLineParser::Option<std::string> prefix(cmd, "prefix", "Prefix to prepend to all file names inside the PAK file", "");
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
		if(mergePaks)
		{
			FileReadStorage inputPakFile(fileName);
			PakHeader inHeader;
			inputPakFile.Read(&inHeader, sizeof(PakHeader));
			std::vector<PakEntry> inEntries(inHeader.dirsize);
			inputPakFile.SetCursor(inHeader.diroffset);
			inputPakFile.Read(inEntries.data(), inHeader.dirsize * sizeof(PakEntry));
			for(auto& inEntry: inEntries)
			{
				inputPakFile.SetCursor(inEntry.offset);
				std::vector<uint8_t> fileData(inEntry.size);
				inputPakFile.Read(fileData.data(), inEntry.size);
				PakEntry entry;
				memset(entry.filename, 0, 56);
				snprintf(entry.filename, 56, "%s%s", prefix->c_str(), inEntry.filename);
				entry.size = inEntry.size;
				entry.offset = outFile.GetCursor();
				entries.push_back(entry);
				outFile.Write(fileData.data(), fileData.size());
			}
		}
		else
		{
			FileReadStorage file(fileName);
			size_t size = file.GetSize();
			std::vector<uint8_t> fileData(size);
			file.Read(fileData.data(), size);

			PakEntry entry;
			memset(entry.filename, 0, 56);
			snprintf(entry.filename, 56, "%s%s", prefix->c_str(), fileName.c_str());
			entry.size = size;
			entry.offset = outFile.GetCursor();
			entries.push_back(entry);
			outFile.Write(fileData.data(), fileData.size());
		}
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
