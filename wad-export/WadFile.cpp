#include "WadFile.h"

struct Header
{
	uint8_t magic[4];
	uint32_t numEntries;
	uint32_t dirOffset;
};

WadFile::WadFile(molecular::util::RandomAccessWriteStorage& storage) :
	mStorage(storage)
{
	Header header;
	storage.Write(&header, sizeof(Header));
}

void WadFile::AddFile(const char name[], FileType type, const uint8_t* data, size_t size)
{
	WadEntry entry;
	entry.offset = mStorage.GetCursor();
	entry.dsize = size;
	entry.size = size;
	entry.type = type;
	entry.cmprs = 0;
	entry.dummy = 0;
	memset(entry.name, 0, 16);
	strncpy(entry.name, name, 16);
	mEntries.push_back(entry);

	mStorage.Write(data, size);
}

void WadFile::Finish()
{
	Header header;
	header.magic[0] = 'W';
	header.magic[1] = 'A';
	header.magic[2] = 'D';
	header.magic[3] = '2';
	header.dirOffset = mStorage.GetCursor();
	header.numEntries = mEntries.size();

	mStorage.Write(mEntries.data(), mEntries.size() * sizeof(WadEntry));
	mStorage.SetCursor(0);
	mStorage.Write(&header, sizeof(Header));
}
