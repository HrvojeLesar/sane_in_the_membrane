cmake_minimum_required(VERSION 4.2)

project(server)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(avahi-client REQUIRED IMPORTED_TARGET avahi-client)

add_library(server_lib STATIC ${SRCFILES})
target_link_libraries(server_lib
    PUBLIC
        sane_in_the_membrane_lib
        sane_in_the_membrane_utils_lib
        generated_protobufs
        PkgConfig::avahi-client
)

add_executable(Server src/main.cpp)
target_link_libraries(Server
    PUBLIC
        server_lib
)

target_compile_options(Server PRIVATE ${COMPILER_WARNING_FLAGS})
