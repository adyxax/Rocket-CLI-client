file(GLOB_RECURSE SOURCES *.c)

ADD_LIBRARY(restapi STATIC ${SOURCES})
add_dependencies(restapi cjson-build)
target_link_libraries(restapi common)
target_link_libraries(restapi ${CMAKE_CURRENT_BINARY_DIR}/../cjson/src/cjson-build/libcjson.a)
