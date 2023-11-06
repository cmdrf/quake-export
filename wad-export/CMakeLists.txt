add_executable(quake-wad-export
	WadExportMain.cpp
	WadFile.cpp
	WadFile.h
)

target_link_libraries(quake-wad-export PRIVATE
	quake-export
)

install(TARGETS quake-wad-export DESTINATION bin)
