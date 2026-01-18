cmake_minimum_required(VERSION 3.12)

project(sane_in_the_membrane)

file(GLOB_RECURSE SRCFILES "src/*.cpp")
file(GLOB_RECURSE PUBLIC_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

# add_library(sane_in_the_membrane_lib STATIC ${SRCFILES})
add_library(sane_in_the_membrane_lib STATIC ${PUBLIC_HEADERS} ${SRCFILES})
target_include_directories(sane_in_the_membrane_lib
    PUBLIC "./include"
)
