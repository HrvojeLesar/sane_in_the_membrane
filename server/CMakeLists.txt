cmake_minimum_required(VERSION 4.2)

project(server)

file(GLOB_RECURSE SRCFILES "src/*.cpp")
list(REMOVE_ITEM SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(avahi-client REQUIRED IMPORTED_TARGET avahi-client)

add_library(server_lib STATIC ${SRCFILES})
target_link_libraries(server_lib
    PUBLIC
        sane_in_the_membrane_test_lib
        sane_in_the_membrane_utils_lib
        generated_protobufs
        PkgConfig::avahi-client
)

if (BUILD_TESTS)
    add_library(server_lib_test STATIC ${SRCFILES})
    target_link_libraries(server_lib_test
        PUBLIC
            sane_in_the_membrane_test_lib
            sane_in_the_membrane_utils_lib
            generated_protobufs
            PkgConfig::avahi-client
    )
endif()

add_executable(Server src/main.cpp)
target_link_libraries(Server
    PUBLIC
        server_lib
)

target_compile_options(Server PRIVATE ${COMPILER_WARNING_FLAGS})

target_include_directories(server_lib
    SYSTEM PUBLIC
        ${grpc_SOURCE_DIR}/third_party/abseil-cpp
)
