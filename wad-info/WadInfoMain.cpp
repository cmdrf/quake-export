#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/LittleEndianStream.h>
#include <molecular/util/Vector3.h>

#include <cstdint>

using namespace molecular::util;

struct Header
{
	uint8_t magic[4];
	uint32_t numEntries;
	uint32_t dirOffset;
};

struct WadEntry
{
	uint32_t offset; ///< Position of the entry in WAD
	uint32_t dsize;  ///< Size of the entry in WAD file
	uint32_t size;   ///< Size of the entry in memory
	uint8_t type;   ///< type of entry
	uint8_t cmprs;  ///< Compression. 0 if none.
	uint16_t dummy; ///< Not used
	char name[16];
};

const char* TypeToString(uint8_t type)
{
	switch(type)
	{
		case 0x40: return "Color Palette ";
		case 0x42: return "Status Bar Pic";
		case 0x44: return "MIP Texture   ";
		case 0x45: return "Console Pic   ";
	}
	return "              ";
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cerr << "Usage: quake-wad-info <WAD file>" << std::endl;
		return EXIT_FAILURE;
	}

	FileReadStorage file(argv[1]);
	Header header;
	file.Read(&header, sizeof(Header));

	if(header.magic[0] != 'W' || header.magic[1] != 'A' || header.magic[2] != 'D' || header.magic[3] != '2')
	{
		std::cerr << "Not a WAD file" << std::endl;
		return EXIT_FAILURE;
	}

	file.Skip(header.dirOffset - sizeof(Header));

	std::cout << "Name             | Type           | Compr | Size\n";
	for(unsigned int i = 0; i < header.numEntries; ++i)
	{
		WadEntry entry;
		file.Read(&entry, sizeof(WadEntry));

		char name[17] = {"                "};
		for(int i = 0; i <= 16 && entry.name[i] != 0; ++i)
			name[i] = entry.name[i];
		std::cout << name << " | " << TypeToString(entry.type) << " | " << (entry.cmprs ? "Yes" : "No ") << "   | " << entry.dsize << std::endl;
	}

	return EXIT_SUCCESS;
}
