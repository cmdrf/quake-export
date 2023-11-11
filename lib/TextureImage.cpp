#include "TextureImage.h"
#include "PaletteImage.h"
#include "Transparency.h"

#include <molecular/util/FileStreamStorage.h>

#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace molecular::util;

TextureImage::TextureImage(const char* filename)
{
	FileReadStorage storage(filename);
	size_t fileSize = storage.GetSize();
	if(fileSize > 8)
	{
		uint32_t width, height;
		storage.Read(&width, 4);
		storage.Read(&height, 4);
		if(width * height + 8 == fileSize)
		{
			mIndexedImage.resize(width * height);
			storage.Read(mIndexedImage.data(), width * height);
			return;
		}
	}

	if(stbi_is_hdr(filename))
		mHdrImage = std::make_unique<StbHdrImage>(filename, 3);
	else
		mImage = std::make_unique<StbImage>(filename, 4);

}

static std::vector<uint8_t> HdrToIndexed(const StbHdrImage& hdrImage, const uint8_t* palette, int mipLevel, float hdrScale)
{
	const float* data = hdrImage.Data();
	int32_t width = hdrImage.GetWidth();
	int32_t height = hdrImage.GetHeight();

	std::vector<float> scaledImage;
	if(mipLevel != 0)
	{
		// Scale down by 2, 4, 8 etc. for higher MIP levels:
		int32_t newWidth = width;
		int32_t newHeight = height;
		for(int i = 0; i < mipLevel; ++i)
		{
			newWidth /= 2;
			newHeight /= 2;
		}
		scaledImage.resize(newWidth * newHeight * 4);
		stbir_resize_float_generic(data, width, height, 0, scaledImage.data(), newWidth, newHeight, 0, 3, STBIR_ALPHA_CHANNEL_NONE, 0, STBIR_EDGE_WRAP, STBIR_FILTER_DEFAULT, STBIR_COLORSPACE_LINEAR, nullptr);

		// Use new image data and size from here:
		data = scaledImage.data();
		width = newWidth;
		height = newHeight;
	}

	std::vector<uint8_t> indexedImage(width * height);

	// Iterate over all pixels:
	for(int i = 0; i < width * height; ++i)
	{
		float rgb[3] = {
			std::pow(data[i*3] * hdrScale, 2.2f) * 255.f + 0.5f,
			std::pow(data[i*3+1] * hdrScale, 2.2f) * 255.f + 0.5f,
			std::pow(data[i*3+2] * hdrScale, 2.2f) * 255.f + 0.5f
		};

		// Colors above 255,255,255 map to fullbright part of palette:
		if(rgb[0] > 255.f || rgb[1] > 255.f || rgb[2] > 255.f)
		{
			const float fullbrightOffset = 128.f; // Map colors back into 0..255
			uint8_t rgbi[3] = {
				static_cast<uint8_t>(std::clamp<int>(rgb[0] - fullbrightOffset, 0, 255)),
				static_cast<uint8_t>(std::clamp<int>(rgb[1] - fullbrightOffset, 0, 255)),
				static_cast<uint8_t>(std::clamp<int>(rgb[2] - fullbrightOffset, 0, 255)),
			};
			// Indices 240 to 254 are for fullbright colors:
			indexedImage[i] = FindClosestPaletteColor(rgbi, palette + 240*3, 15) + 240;
		}
		else
		{
			uint8_t rgbi[3] = {
				static_cast<uint8_t>(std::clamp<int>(rgb[0], 0, 255)),
				static_cast<uint8_t>(std::clamp<int>(rgb[1], 0, 255)),
				static_cast<uint8_t>(std::clamp<int>(rgb[2], 0, 255)),
			};
			indexedImage[i] = FindClosestPaletteColor(rgbi, palette, 240);
		}
	}
	return indexedImage;
}

static std::vector<uint8_t> LdrToIndexed(const StbImage& image, const uint8_t* palette, bool dither, int mipLevel)
{
	int32_t width = image.GetWidth();
	int32_t height = image.GetHeight();
	const uint8_t* imageData = image.Data();
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
		stbir_resize_uint8_srgb_edgemode(image.Data(), width, height, width * 4, scaledImage.data(), newWidth, newHeight, newWidth * 4, 4, 3, 0, STBIR_EDGE_WRAP);
		imageData = scaledImage.data();
		width = newWidth;
		height = newHeight;
	}

	auto [color, alpha] = SplitColorAndAlpha(imageData, width, height);
	auto indexedImage = ConvertToIndexed(color.data(), width, height, palette, dither);
	SetTransparency(indexedImage, alpha);
	return indexedImage;
}

std::vector<uint8_t> TextureImage::ToIndexed(const uint8_t* palette, bool dither, int mipLevel, float hdrScale)
{
	std::vector<uint8_t> indexedImage;
	if(mHdrImage)
	{
		if(dither)
			throw std::runtime_error("Dither with HDR not supported");

		indexedImage = HdrToIndexed(*mHdrImage, palette, mipLevel, hdrScale);
	}
	else if(mImage)
	{
		indexedImage = LdrToIndexed(*mImage, palette, dither, mipLevel);
	}
	else if(!mIndexedImage.empty())
	{
		if(mipLevel != 0)
			throw std::runtime_error("Cannot use picture lump as MIP texture");
		if(dither)
			throw std::runtime_error("Cannot dither already indexed image");
		return mIndexedImage;
	}
	else
		throw std::runtime_error("No texture image loaded");
	return indexedImage;
}
