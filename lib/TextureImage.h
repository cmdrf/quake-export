#ifndef TEXTUREIMAGE_H
#define TEXTUREIMAGE_H

#include "StbImage.h"

#include <vector>

class TextureImage
{
public:
	TextureImage(const char* filename);

	int GetWidth() const {return mImage.GetWidth();}
	int GetHeight() const {return mImage.GetHeight();}

	std::vector<uint8_t> ToIndexed(const uint8_t* palette, bool dither, int mipLevel = 0);

private:
	StbImage mImage;
};

#endif // TEXTUREIMAGE_H
