cmake_minimum_required(VERSION 3.12)

project(gui-app)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

add_library(gui_lib STATIC ${SRCFILES})
add_executable(Gui src/main.cpp)
target_link_libraries(Gui gui_lib)
