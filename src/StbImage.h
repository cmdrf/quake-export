#ifndef STBIMAGE_H
#define STBIMAGE_H

#include <cstdint>

/// Minimal C++ wrapper around stb_image
class StbImage
{
public:
	StbImage(char const *filename, int desired_channels);
	StbImage(StbImage&) = delete;
	~StbImage();

	const uint8_t* Data() const {return mData;}
	int GetWidth() const {return mWidth;}
	int GetHeight() const {return mHeight;}
	int GetChannelsInFile() const {return mChannelsInFile;}

private:
	uint8_t* mData;
	int mWidth = 0;
	int mHeight = 0;
	int mChannelsInFile = 0;
};

#endif // STBIMAGE_H