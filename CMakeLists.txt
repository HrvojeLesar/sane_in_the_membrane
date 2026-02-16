cmake_minimum_required(VERSION 4.2)

project(sane_in_the_membrane LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

OPTION(WITH_ASAN "Compiles binary with asan" OFF)
OPTION(BUILD_GUI "Builds gui executable" ON)
OPTION(BUILD_SERVER "Builds server executable" ON)

# This it not portable across platforms
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DEBUG)
    message(STATUS "CMake DEBUG mode")
    if (WITH_ASAN)
        message("Enabled ASAN")
        add_compile_options(
        -fsanitize=address
        -fsanitize=undefined
        -fsanitize=leak
    )
        add_link_options(
        -fsanitize=address
        -fsanitize=undefined
        -fsanitize=leak
    )
    endif()
else()
    message(STATUS "CMake RELEASE mode")
    if (NOT MSVC)
        add_compile_options(-O3)
    endif()
endif()

# This it not portable across platforms
if (NOT MSVC)
    set(COMPILER_WARNING_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    -Wno-unused-value
    -Wno-missing-field-initializers
    -Wno-narrowing
    -Werror
    )
endif()

add_subdirectory(utils)
add_subdirectory(proto_lib)

if(BUILD_GUI)
    add_subdirectory(gui-app)
endif()

if(BUILD_SERVER AND NOT (WIN32 OR MINGW))
    add_subdirectory(sane_wrapper)
    add_subdirectory(server)
endif()
