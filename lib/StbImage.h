#ifndef STBIMAGE_H
#define STBIMAGE_H

#include <cstddef>
#include <cstdint>

/// Minimal C++ wrapper around stb_image
class StbImage
{
public:
	/// Load from file given by name
	StbImage(char const *filename, int desired_channels);

	/// Load from memory
	StbImage(uint8_t const* data, size_t size, int desiredChannels);

	StbImage(const StbImage&) = delete;
	StbImage(StbImage&& other);
	~StbImage();

	StbImage& operator=(const StbImage&) = delete;
	StbImage& operator=(StbImage&& other);

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
