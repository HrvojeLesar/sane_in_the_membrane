cmake_minimum_required(VERSION 3.12)

project(server)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

add_library(server_lib STATIC ${SRCFILES})
add_executable(Server src/main.cpp)
target_link_libraries(Server server_lib)
