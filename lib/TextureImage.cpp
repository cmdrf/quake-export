#include "TextureImage.h"
#include "PaletteImage.h"
#include "Transparency.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

TextureImage::TextureImage(const char* filename) :
	mImage(filename, 4)
{

}

std::vector<uint8_t> TextureImage::ToIndexed(const uint8_t* palette, bool dither, int mipLevel)
{
	int32_t width = mImage.GetWidth();
	int32_t height = mImage.GetHeight();
	const uint8_t* imageData = mImage.Data();
	std::vector<uint8_t> scaledImage;

	if(mipLevel != 0)
	{
		int32_t newWidth = width;
		int32_t newHeight = height;
		for(int i = 0; i < mipLevel; ++i)
		{
			newWidth /= 2;
			newHeight /= 2;
		}
		scaledImage.resize(newWidth * newHeight * 4);
		stbir_resize_uint8_srgb_edgemode(mImage.Data(), width, height, width * 4, scaledImage.data(), newWidth, newHeight, newWidth * 4, 4, 3, 0, STBIR_EDGE_WRAP);
		imageData = scaledImage.data();
		width = newWidth;
		height = newHeight;
	}

	auto [color, alpha] = SplitColorAndAlpha(imageData, width, height);
	std::vector<uint8_t> indexedImage = ConvertToIndexed(color.data(), width, height, palette, dither);
	SetTransparency(indexedImage, alpha);
	return indexedImage;
}
