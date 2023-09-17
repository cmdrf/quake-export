#include "WriteImage.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <molecular/util/StringUtils.h>

#include <stdexcept>

using namespace molecular::util::StringUtils;

void WriteRgbImage(const char* path, const uint8_t* data, int width, int height)
{
	int result = 0;
	if(EndsWith(path, ".jpg") || EndsWith(path, ".jpeg"))
		result = stbi_write_jpg(path, width, height, 3, data, 90);
	else if(EndsWith(path, ".tga"))
		result = stbi_write_tga(path, width, height, 3, data);
	else if(EndsWith(path, ".png"))
		result = stbi_write_png(path, width, height, 3, data, width * 3);
	else if(EndsWith(path, ".bmp"))
		result = stbi_write_bmp(path, width, height, 3, data);
	else
		throw std::runtime_error("Unrecognized image file extension");

	if(result == 0)
		throw std::runtime_error(std::string("Error writing image file ") + path);
}
