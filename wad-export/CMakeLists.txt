add_executable(wad-export
	WadExportMain.cpp
	WadFile.cpp
	WadFile.h
)

target_link_libraries(wad-export PRIVATE
	quake-export
)

install(TARGETS wad-export DESTINATION bin)
