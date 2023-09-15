#include "StbImage.h"

#ifdef __ARM_NEON
#define STBI_NEON
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>
#include <string>

StbImage::StbImage(const char* filename, int desired_channels)
{
	mData = stbi_load(filename, &mWidth, &mHeight, &mChannelsInFile, desired_channels);
	if(!mData)
		throw std::runtime_error(std::string("Could not open image file ") + filename);
}

StbImage::StbImage(uint8_t const* data, size_t size, int desiredChannels)
{
	mData = stbi_load_from_memory(data, size, &mWidth, &mHeight, &mChannelsInFile, desiredChannels);
	if(!mData)
		throw std::runtime_error("Could not load image data");
}

StbImage::~StbImage()
{
	stbi_image_free(mData);
}
