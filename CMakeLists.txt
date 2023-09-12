cmake_minimum_required(VERSION 3.15)
project(mdl-export)

set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(molecular-util)
add_subdirectory(lib)
add_subdirectory(mdl-export)

