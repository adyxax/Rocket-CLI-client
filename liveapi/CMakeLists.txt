list(APPEND CMAKE_ARGS "-DUWSC_LUA_SUPPORT=OFF")
ExternalProject_Add(libuwsc
    PREFIX ${CMAKE_BINARY_DIR}/libuwsc
    GIT_REPOSITORY https://github.com/zhaojh329/libuwsc
    CMAKE_ARGS "${CMAKE_ARGS}"
    STEP_TARGETS build
    EXCLUDE_FROM_ALL TRUE
    )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libuwsc)

file(GLOB_RECURSE SOURCES *.c)

ADD_LIBRARY(liveapi STATIC ${SOURCES})
add_dependencies(liveapi libuwsc-build)
target_link_libraries(liveapi common)
target_link_libraries(liveapi crypto ev ssl)
target_link_libraries(liveapi ${CMAKE_CURRENT_BINARY_DIR}/../libuwsc/src/libuwsc-build/src/libuwsc.a)
target_link_libraries(liveapi ${CMAKE_CURRENT_BINARY_DIR}/../cjson/src/cjson-build/libcjson.a)
