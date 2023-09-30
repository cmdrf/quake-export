#include "Transparency.h"

std::pair<std::vector<uint8_t>, std::vector<uint8_t>> SplitColorAndAlpha(const uint8_t* data, int width, int height)
{
	std::pair<std::vector<uint8_t>, std::vector<uint8_t>> out;
	out.first.resize(width * height * 3);
	out.second.resize(width * height);
	for(int i = 0; i < width * height; ++i)
	{
		out.first[i * 3] = data[i * 4];
		out.first[i * 3 + 1] = data[i * 4 + 1];
		out.first[i * 3 + 2] = data[i * 4 + 2];
		out.second[i] = data[i * 4 + 3];
	}
	return out;
}

void SetTransparency(std::vector<uint8_t>& indexedImage, const std::vector<uint8_t>& alpha)
{
	assert(indexedImage.size() == alpha.size());

	for(size_t i = 0; i < indexedImage.size(); ++i)
	{
		if(alpha[i] < 128)
			indexedImage[i] = 255;
	}
}
