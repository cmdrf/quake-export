#ifndef MIPTEXFILE_H
#define MIPTEXFILE_H

#include <molecular/util/StreamStorage.h>

#include <cstdint>

class MiptexFile
{
public:
	MiptexFile(molecular::util::WriteStorage& storage);

	void WriteHeader(const char name[16], unsigned int width, unsigned int height);
	void WriteMip(const uint8_t* data, size_t dataSize);

private:

	molecular::util::WriteStorage& mStorage;
};

#endif // MIPTEXFILE_H
