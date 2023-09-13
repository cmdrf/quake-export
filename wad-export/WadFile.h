#ifndef WADFILE_H
#define WADFILE_H

#include <molecular/util/StreamStorage.h>

#include <vector>

class WadFile
{
public:
	enum FileType
	{
		COLOR_PALETTE = 0x40, ///< Color Palette
		STATUSBAR_PIC = 0x42,	///< Pictures for status bar
		MIPTEX = 0x44, ///< MIP Texture
		CONSOLE_PIC = 0x45
	};

	WadFile(molecular::util::RandomAccessWriteStorage& storage);

	void AddFile(const char name[16], FileType type, const uint8_t* data, size_t size);
	void Finish();

private:
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

	molecular::util::RandomAccessWriteStorage& mStorage;
	std::vector<WadEntry> mEntries;
};

#endif // WADFILE_H
