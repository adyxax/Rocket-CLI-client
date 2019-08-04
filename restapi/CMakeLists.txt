file(GLOB_RECURSE SOURCES *.c)

ADD_LIBRARY(restapi STATIC ${SOURCES})
add_dependencies(restapi cjson-build)
target_link_libraries(restapi common)
