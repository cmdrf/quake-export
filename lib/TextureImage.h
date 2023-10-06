#ifndef TEXTUREIMAGE_H
#define TEXTUREIMAGE_H

#include "StbImage.h"
#include "StbHdrImage.h"

#include <vector>

class TextureImage
{
public:
	TextureImage(const char* filename);

	int GetWidth() const {return mImage ? mImage->GetWidth() : mHdrImage->GetWidth();}
	int GetHeight() const {return mImage ? mImage->GetHeight() : mHdrImage->GetHeight();}

	std::vector<uint8_t> ToIndexed(const uint8_t* palette, bool dither, int mipLevel = 0, float hdrScale = 1);

private:
	std::unique_ptr<StbImage> mImage;
	std::unique_ptr<StbHdrImage> mHdrImage;
};

#endif // TEXTUREIMAGE_H
