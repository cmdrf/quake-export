add_executable(quake-mdl-info
	MdlInfoMain.cpp
)

target_link_libraries(quake-mdl-info PUBLIC
	molecular::util
)

install(TARGETS quake-mdl-info DESTINATION bin)
