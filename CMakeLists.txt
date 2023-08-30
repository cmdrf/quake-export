cmake_minimum_required(VERSION 3.15)
project(mdl-export)

set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(molecular-util)

add_executable(mdl-export
	src/MdlExportMain.cpp
	src/MdlFile.cpp
	src/MdlFile.h
	src/PaletteImage.cpp
	src/PaletteImage.h
	src/QuakeNormal.cpp
	src/QuakeNormal.h
	src/QuakePalette.cpp
	src/QuakePalette.h
	src/stb_image.cpp
	src/stb_image.h
)

target_link_libraries(mdl-export PRIVATE
	molecular::util
)
