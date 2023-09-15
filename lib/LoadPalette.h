#ifndef LOADPALETTE_H
#define LOADPALETTE_H

#include <vector>

/// Load palette from file
/** Can be a 16x16 image or a Quake .lmp file. */
std::vector<uint8_t> LoadPaletteFile(const char* filename);

#endif // LOADPALETTE_H
