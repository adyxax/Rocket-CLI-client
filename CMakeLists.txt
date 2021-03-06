cmake_minimum_required(VERSION 3.0)
project(rocket-client LANGUAGES C VERSION 0.0.1)

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

set(CMAKE_C_COMPILER "clang")
set(CMAKE_C_FLAGS "-Wall -Werror -Wextra -pedantic -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=10000 -D_XOPEN_SOURCE_EXTENDED -Wno-variadic-macros")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -ggdb -pg -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_MinSizeRel "-Os")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -ggdb -pg -fsanitize=address")

option(DEBUG_WEBSOCKETS "whether or not to print all incoming and outgoing websocket communications" OFF)
if(DEBUG_WEBSOCKETS)
    add_definitions(-DDEBUG_WEBSOCKETS)
endif()

execute_process(COMMAND git rev-parse HEAD RESULT_VARIABLE GIT_HASH_RESULT
    OUTPUT_VARIABLE GIT_HASH_FULL)
string(STRIP ${GIT_HASH_FULL} GIT_HASH)
configure_file("common/config.h.in" "common/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

### cjson library #####
include(ExternalProject)
list(APPEND CMAKE_ARGS "-DBUILD_SHARED_LIBS=OFF")
list(APPEND CMAKE_ARGS "-DENABLE_CUSTOM_COMPILER_FLAGS=OFF")
list(APPEND CMAKE_ARGS "-DENABLE_CJSON_TEST=OFF")
ExternalProject_Add(cjson
    PREFIX ${CMAKE_BINARY_DIR}/cjson
    GIT_REPOSITORY https://github.com/DaveGamble/cJSON
    CMAKE_ARGS "${CMAKE_ARGS}"
    STEP_TARGETS build
    EXCLUDE_FROM_ALL TRUE
    )
include_directories(${CMAKE_BINARY_DIR}/cjson/src)

### uthash library #####
include_directories("${PROJECT_SOURCE_DIR}/external/uthash/src")

### Project subdirectories #####
add_subdirectory(close_direct_conversations)
add_subdirectory(common)
add_subdirectory(liveapi)
add_subdirectory(maar)
add_subdirectory(restapi)
