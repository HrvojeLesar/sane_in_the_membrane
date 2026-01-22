cmake_minimum_required(VERSION 3.12)

project(server)

find_package(PkgConfig REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf REQUIRED)

pkg_check_modules(SANE REQUIRED IMPORTED_TARGET sane-backends)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

add_library(server_lib STATIC ${SRCFILES})
target_link_libraries(server_lib
    PUBLIC
        sane_in_the_membrane_lib
        generated_protobufs
)

add_executable(Server src/main.cpp)
target_link_libraries(Server
    PUBLIC
        server_lib
    PRIVATE
        PkgConfig::SANE
)
