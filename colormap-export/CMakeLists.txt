add_executable(quake-colormap-export
	ColormapExportMain.cpp
)

target_link_libraries(quake-colormap-export PRIVATE
	quake-export
)

install(TARGETS quake-colormap-export DESTINATION bin)
