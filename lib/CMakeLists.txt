add_library(quake-export
	LoadPalette.cpp
	LoadPalette.h
	PaletteImage.cpp
	PaletteImage.h
	QuakePalette.cpp
	QuakePalette.h
	StbHdrImage.cpp
	StbHdrImage.h
	StbImage.cpp
	StbImage.h
	TextureImage.cpp
	TextureImage.h
	Transparency.cpp
	Transparency.h
	WriteImage.cpp
	WriteImage.h
	../3rdparty/stb_image.h
	../3rdparty/stb_image_write.h
)

target_link_libraries(quake-export PUBLIC
	molecular::util
)

target_include_directories(quake-export PUBLIC
	../3rdparty
	.
)
