add_executable(miptex-export
	MiptexExportMain.cpp
	MiptexFile.cpp
	MiptexFile.h
)

target_link_libraries(miptex-export PRIVATE
	quake-export
)

install(TARGETS miptex-export DESTINATION bin)
