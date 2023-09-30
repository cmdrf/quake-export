add_executable(quake-picture-export
	PictureExportMain.cpp
)

target_link_libraries(quake-picture-export PRIVATE
	quake-export
)

install(TARGETS quake-picture-export DESTINATION bin)
