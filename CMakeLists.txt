cmake_minimum_required(VERSION 3.15)
project(quake-export)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(molecular-util)
add_subdirectory(lib)
add_subdirectory(colormap-export)
add_subdirectory(hdr-split)
add_subdirectory(mdl-export)
add_subdirectory(mdl-info)
add_subdirectory(miptex-export)
add_subdirectory(pak-export)
add_subdirectory(picture-export)
add_subdirectory(picture-import)
add_subdirectory(wad-export)
add_subdirectory(wad-info)

