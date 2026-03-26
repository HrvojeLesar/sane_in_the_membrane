cmake_minimum_required(VERSION 4.2)

project(serverctl)

# file(GLOB_RECURSE SRCFILES "src/*.cpp")
# list(REMOVE_ITEM SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
# add_library(serverctl_lib STATIC ${SRCFILES})

add_executable(serverctl src/main.cpp)
target_link_libraries(serverctl
    PUBLIC
        sane_in_the_membrane_utils_lib
)
