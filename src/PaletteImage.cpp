/*	PaletteImage.cpp

MIT License

Copyright (c) 2023 Fabian Herb

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "PaletteImage.h"

#include <cmath>
#include <algorithm>

struct RGB
{
	int r, g, b;

	RGB operator-(const RGB& other) const
	{
		return { r - other.r, g - other.g, b - other.b };
	}

	RGB operator+(const RGB& other) const
	{
		return { r + other.r, g + other.g, b + other.b };
	}

	RGB operator*(double factor) const
	{
		return { static_cast<int>(r * factor),
					static_cast<int>(g * factor),
					static_cast<int>(b * factor) };
	}

	void Clip()
	{
		r = std::clamp(r, 0, 255);
		g = std::clamp(g, 0, 255);
		b = std::clamp(b, 0, 255);
	}
};

static double Distance(const RGB& a, const RGB& b)
{
	return sqrt((a.r - b.r) * (a.r - b.r) +
				(a.g - b.g) * (a.g - b.g) +
				(a.b - b.b) * (a.b - b.b));
}

uint8_t FindClosestPaletteColor(const RGB& color, const std::vector<RGB>& palette)
{
	double minDist = 1e9;
	uint8_t index = 0;
	for (int i = 0; i < palette.size(); i++)
	{
		double dist = Distance(color, palette[i]);
		if (dist < minDist)
		{
			minDist = dist;
			index = i;
		}
	}
	return index;
}

std::vector<uint8_t> ConvertToIndexed(const std::vector<RGB>& image, int width, int height, const std::vector<RGB>& palette, bool dither)
{
	std::vector<uint8_t> indexedImage(width * height, 0);

	if(dither)
	{
		std::vector<RGB> ditheredImage = image;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				RGB oldColor = ditheredImage[y * width + x];
				int paletteIndex = FindClosestPaletteColor(oldColor, palette);
				RGB newColor = palette[paletteIndex];
				indexedImage[y * width + x] = paletteIndex;

				RGB error = oldColor - newColor;

				if (x + 1 < width)
				{
					ditheredImage[y * width + (x + 1)] = ditheredImage[y * width + (x + 1)] + error * (7.0 / 16.0);
					ditheredImage[y * width + (x + 1)].Clip();
				}
				if (y + 1 < height)
				{
					if (x > 0)
					{
						ditheredImage[(y + 1) * width + (x - 1)] = ditheredImage[(y + 1) * width + (x - 1)] + error * (3.0 / 16.0);
						ditheredImage[(y + 1) * width + (x - 1)].Clip();
					}
					ditheredImage[(y + 1) * width + x] = ditheredImage[(y + 1) * width + x] + error * (5.0 / 16.0);
					ditheredImage[(y + 1) * width + x].Clip();
					if (x + 1 < width)
					{
						ditheredImage[(y + 1) * width + (x + 1)] = ditheredImage[(y + 1) * width + (x + 1)] + error * (1.0 / 16.0);
						ditheredImage[(y + 1) * width + (x + 1)].Clip();
					}
				}
			}
		}
	}
	else
	{
		for(size_t i = 0; i < image.size(); ++i)
			indexedImage[i] = FindClosestPaletteColor(image[i], palette);
	}

	return indexedImage;
}

std::vector<uint8_t> ConvertToIndexed(const uint8_t* image, int width, int height, const uint8_t* palette, bool dither)
{
	std::vector<RGB> rgbImage(width * height);
	for(int i = 0; i < width * height; ++i)
		rgbImage[i] = RGB{image[3*i], image[3*i + 1], image[3*i + 2]};
	std::vector<RGB> rgbPalette(240);
	for(int i = 0; i < 240; ++i)
		rgbPalette[i] = RGB{palette[3*i], palette[3*i + 1], palette[3*i + 2]};
	return ConvertToIndexed(rgbImage, width, height, rgbPalette, dither);
}
