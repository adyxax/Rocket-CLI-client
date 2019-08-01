cmake_minimum_required(VERSION 3.0)
project(rocket-client LANGUAGES C CXX VERSION 0.0.1)

set(CMAKE_VERBOSE_MAKEFILE FALSE)
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(default_build_type Debug)
else()
    set(default_build_type Release)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra -pedantic -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -ggdb -pg -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_MinSizeRel "-Os")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -ggdb -pg -fsanitize=address")

execute_process(COMMAND git rev-parse HEAD RESULT_VARIABLE GIT_HASH_RESULT
    OUTPUT_VARIABLE GIT_HASH_FULL)
string(STRIP ${GIT_HASH_FULL} GIT_HASH)
configure_file("common/config.h.in" "common/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(common)
