file(GLOB_RECURSE SOURCES *.c)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

ADD_LIBRARY(common STATIC ${SOURCES})
target_link_libraries(common config curl)
