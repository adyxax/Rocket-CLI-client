file(GLOB_RECURSE SOURCES *.cpp)

ADD_EXECUTABLE(rocket_close_im ${SOURCES})
target_link_libraries(rocket_close_im common rest)
target_link_libraries(rocket_close_im curl)

install(TARGETS rocket_close_im DESTINATION bin)
