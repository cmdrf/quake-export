#include "LoadPalette.h"
#include "StbImage.h"

#include <molecular/util/FileStreamStorage.h>

using namespace molecular::util;

static std::vector<uint8_t> TryLoadFromImage(const char* filename)
{
	std::vector<uint8_t> palette;
	int width = 0;
	int height = 0;
	try
	{
		StbImage image(filename, 3);
		width = image.GetWidth();
		height = image.GetHeight();
		if(width == 16 && height == 16)
		{
			palette.resize(width * height * 3);
			std::memcpy(palette.data(), image.Data(), width * height * 3);
		}
	}
	catch(...)
	{
		return std::vector<uint8_t>();
	}

	if(width != 16 || height != 16)
		throw std::runtime_error("Palette image must be 16x16 pixels in size");
	return std::vector<uint8_t>();
}

std::vector<uint8_t> LoadPaletteFile(const char* filename)
{
	std::vector<uint8_t> palette = TryLoadFromImage(filename);
	if(palette.empty())
	{
		FileReadStorage storage(filename);
		if(storage.GetSize() != 768)
			throw std::runtime_error("Palette file doesn't seem to be a Quake palette lump file");
		palette.resize(768);
		storage.Read(palette.data(), 768);
	}
	return palette;
}
