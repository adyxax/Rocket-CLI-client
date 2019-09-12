file(GLOB_RECURSE SOURCES *.c)

ADD_EXECUTABLE(rocket_maar ${SOURCES})
target_link_libraries(rocket_maar common liveapi restapi)

install(TARGETS rocket_maar DESTINATION bin)
