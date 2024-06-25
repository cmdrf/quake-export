add_executable(hdr-split
	HdrSplitMain.cpp
)

target_link_libraries(hdr-split
	quake-export
)

install(TARGETS hdr-split DESTINATION bin)
