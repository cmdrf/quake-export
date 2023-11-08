add_executable(quake-pak-export
	PakExportMain.cpp
)

target_link_libraries(quake-pak-export PRIVATE
	quake-export
)

install(TARGETS quake-pak-export DESTINATION bin)
