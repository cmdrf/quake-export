add_executable(quake-mdl-export
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

target_link_libraries(quake-mdl-export PRIVATE
	quake-export
)

install(TARGETS quake-mdl-export DESTINATION bin)
