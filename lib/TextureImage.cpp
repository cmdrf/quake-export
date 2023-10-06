#include "TextureImage.h"
#include "PaletteImage.h"
#include "Transparency.h"

#include <iostream>
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#include <cmath>

TextureImage::TextureImage(const char* filename)
{
	if(stbi_is_hdr(filename))
		mHdrImage = std::make_unique<StbHdrImage>(filename, 3);
	else
		mImage = std::make_unique<StbImage>(filename, 4);

}

std::vector<uint8_t> TextureImage::ToIndexed(const uint8_t* palette, bool dither, int mipLevel)
{
	std::vector<uint8_t> indexedImage;
	if(mHdrImage)
	{
		const float* data = mHdrImage->Data();
		int32_t width = mHdrImage->GetWidth();
		int32_t height = mHdrImage->GetHeight();
		unsigned int pixelCount = width * height;
		std::vector<uint8_t> ldrImage(pixelCount * 3);
		for(int i = 0; i < pixelCount * 3; ++i)
		{
			float value = std::pow(data[i] * 2, 2.2) * 255 + 0.5f;
			if(value < 0)
				ldrImage[i] = 0;
			else if(value > 255.0f)
			{
				ldrImage[i] = 255;
				// TODO: Fullbright
			}
			else
				ldrImage[i] = value;
		}

		const uint8_t* imageData = ldrImage.data();
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
			stbir_resize_uint8_srgb_edgemode(ldrImage.data(), width, height, width * 4, scaledImage.data(), newWidth, newHeight, newWidth * 4, 4, 3, 0, STBIR_EDGE_WRAP);
			imageData = scaledImage.data();
			width = newWidth;
			height = newHeight;
		}

		indexedImage = ConvertToIndexed(imageData, width, height, palette, dither);
	}
	else
	{
		int32_t width = mImage->GetWidth();
		int32_t height = mImage->GetHeight();
		const uint8_t* imageData = mImage->Data();
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
			stbir_resize_uint8_srgb_edgemode(mImage->Data(), width, height, width * 4, scaledImage.data(), newWidth, newHeight, newWidth * 4, 4, 3, 0, STBIR_EDGE_WRAP);
			imageData = scaledImage.data();
			width = newWidth;
			height = newHeight;
		}

		auto [color, alpha] = SplitColorAndAlpha(imageData, width, height);
		indexedImage = ConvertToIndexed(color.data(), width, height, palette, dither);
		SetTransparency(indexedImage, alpha);
	}
	return indexedImage;
}
