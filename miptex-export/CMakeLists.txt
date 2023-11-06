add_executable(quake-miptex-export
	MiptexExportMain.cpp
	MiptexFile.cpp
	MiptexFile.h
)

target_link_libraries(quake-miptex-export PRIVATE
	quake-export
)

install(TARGETS quake-miptex-export DESTINATION bin)
