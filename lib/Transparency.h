#ifndef TRANSPARENCY_H
#define TRANSPARENCY_H

#include <cstdint>
#include <vector>

/// Split RGBA image into RGB and A
std::pair<std::vector<uint8_t>, std::vector<uint8_t>> SplitColorAndAlpha(const uint8_t* data, int width, int height);

/// Set pixel value to 255 where the alpha image is < 50%
void SetTransparency(std::vector<uint8_t>& indexedImage, const std::vector<uint8_t>& alpha);

#endif // TRANSPARENCY_H
