cmake_minimum_required(VERSION 3.15)
project(mdl-export)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(molecular-util)
add_subdirectory(lib)
add_subdirectory(mdl-export)
add_subdirectory(mdl-info)
add_subdirectory(miptex-export)
add_subdirectory(picture-export)
add_subdirectory(wad-export)

