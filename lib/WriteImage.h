#ifndef WRITEIMAGE_H
#define WRITEIMAGE_H

#include <cstdint>

/// Write RGB image file
/** Format is guessed from file extension. */
void WriteRgbImage(const char* path, const uint8_t* data, int width, int height);

#endif // WRITEIMAGE_H
