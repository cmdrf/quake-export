#include "StbHdrImage.h"

#include "stb_image.h"

#include <stdexcept>
#include <string>

StbHdrImage::StbHdrImage(const char* filename, int desired_channels)
{
	mData = stbi_loadf(filename, &mWidth, &mHeight, &mChannelsInFile, desired_channels);
	if(!mData)
		throw std::runtime_error(std::string("Could not open image file ") + filename);

}

StbHdrImage::StbHdrImage(StbHdrImage&& other) :
	mData(other.mData),
	mWidth(other.mWidth),
	mHeight(other.mHeight),
	mChannelsInFile(other.mChannelsInFile)
{
	other.mData = nullptr;
}

StbHdrImage::~StbHdrImage()
{
	if(mData)
		stbi_image_free(mData);
}

StbHdrImage& StbHdrImage::operator=(StbHdrImage&& other)
{
	mData = other.mData;
	mWidth = other.mWidth;
	mHeight = other.mHeight;
	mChannelsInFile = other.mChannelsInFile;
	other.mData = nullptr;
	return *this;
}
