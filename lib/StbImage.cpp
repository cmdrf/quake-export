#include "StbImage.h"

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

StbImage::~StbImage()
{
	stbi_image_free(mData);
}
