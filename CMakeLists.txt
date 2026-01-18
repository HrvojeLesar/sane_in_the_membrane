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

# find_package(PkgConfig REQUIRED)

add_subdirectory(sane_wrapper)
add_subdirectory(gui-app)
add_subdirectory(server)
