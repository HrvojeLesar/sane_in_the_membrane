cmake_minimum_required(VERSION 3.12)

project(unnamed-scanner-project LANGUAGES CXX)

SET(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-unused-value
    -Wno-missing-field-initializers
    -Wno-narrowing)

add_subdirectory(utils)
add_subdirectory(sane_wrapper)
add_subdirectory(proto_lib)

if (NOT SKIP_GUI)
    add_subdirectory(gui-app)
endif()

if (NOT SKIP_SERVER)
    add_subdirectory(server)
endif()
