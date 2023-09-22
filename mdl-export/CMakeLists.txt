add_executable(mdl-export
	MdlExportMain.cpp
	MdlFile.cpp
	MdlFile.h
	MdlJson.cpp
	MdlJson.h
	MdlUtils.cpp
	MdlUtils.h
	QuakeNormal.cpp
	QuakeNormal.h
)

target_link_libraries(mdl-export PRIVATE
	quake-export
)

install(TARGETS mdl-export DESTINATION bin)
