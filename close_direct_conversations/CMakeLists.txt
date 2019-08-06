file(GLOB_RECURSE SOURCES *.c)

ADD_EXECUTABLE(rocket_close_direct_conversations ${SOURCES})
target_link_libraries(rocket_close_direct_conversations common restapi)

install(TARGETS rocket_close_direct_conversations DESTINATION bin)
