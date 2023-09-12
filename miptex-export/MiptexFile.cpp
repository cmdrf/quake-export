#include "MiptexFile.h"

#include <cstring>

struct Header
{
	char name[16]; ///< Name of the texture.
	uint32_t width;   ///< width of picture, must be a multiple of 8
	uint32_t height;  ///< height of picture, must be a multiple of 8
	uint32_t offset1; ///< offset to image[width   * height]
	uint32_t offset2; ///< offset to image[width/2 * height/2]
	uint32_t offset4; ///< offset to image[width/4 * height/4]
	uint32_t offset8; ///< offset to image[width/8 * height/8]
};

MiptexFile::MiptexFile(molecular::util::WriteStorage& storage) :
	mStorage(storage)
{

}

void MiptexFile::WriteHeader(const char name[], unsigned int width, unsigned int height)
{
	Header header;
	strncpy(header.name, name, 16);
	header.width = width;
	header.height = height;
	header.offset1 = sizeof(Header);
	header.offset2 = header.offset1 + width * height;
	header.offset4 = header.offset2 + (width * height) / 4;
	header.offset8 = header.offset4 + (width * height) / 8;
	mStorage.Write(&header, sizeof(Header));
}

void MiptexFile::WriteMip(const uint8_t* data, size_t dataSize)
{
	mStorage.Write(data, dataSize);
}
