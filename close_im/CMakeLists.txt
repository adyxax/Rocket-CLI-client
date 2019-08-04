file(GLOB_RECURSE SOURCES *.c)

ADD_EXECUTABLE(rocket_close_im ${SOURCES})
target_link_libraries(rocket_close_im common restapi)
target_link_libraries(rocket_close_im config curl)
add_dependencies(rocket_close_im cjson-build)
target_link_libraries(rocket_close_im ${CMAKE_CURRENT_BINARY_DIR}/../cjson/src/cjson-build/libcjson.a)

install(TARGETS rocket_close_im DESTINATION bin)
