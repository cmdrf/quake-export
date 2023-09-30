add_executable(quake-picture-import
	PictureImportMain.cpp
)

target_link_libraries(quake-picture-import PRIVATE
	quake-export
)

install(TARGETS quake-picture-import DESTINATION bin)
