add_executable(mdl-export
	MdlExportMain.cpp
	MdlFile.cpp
	MdlFile.h
	QuakeNormal.cpp
	QuakeNormal.h
)

target_link_libraries(mdl-export PRIVATE
	quake-export
)

install(TARGETS mdl-export DESTINATION bin)
