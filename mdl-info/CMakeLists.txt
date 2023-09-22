add_executable(mdl-info
	MdlInfoMain.cpp
)

target_link_libraries(mdl-info PUBLIC
	molecular::util
)

install(TARGETS mdl-info DESTINATION bin)
