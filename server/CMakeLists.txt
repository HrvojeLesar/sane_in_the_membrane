cmake_minimum_required(VERSION 3.12)

project(server)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SANE REQUIRED IMPORTED_TARGET sane-backends)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

add_library(server_lib STATIC ${SRCFILES})
target_link_libraries(server_lib
    PUBLIC
        sane_in_the_membrane_lib
)

add_executable(Server src/main.cpp)
target_link_libraries(Server
    PUBLIC
        server_lib
        sane_in_the_membrane_lib
    PRIVATE
        PkgConfig::SANE
)
