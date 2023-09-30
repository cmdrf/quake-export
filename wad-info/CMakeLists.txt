add_executable(quake-wad-info
	WadInfoMain.cpp
)

target_link_libraries(quake-wad-info PRIVATE
	molecular::util
)

install(TARGETS quake-wad-info DESTINATION bin)
