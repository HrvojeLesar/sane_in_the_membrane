cmake_minimum_required(VERSION 4.2)

project(sane_in_the_membrane LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

OPTION(WITH_ASAN "Compiles binary with asan" OFF)
OPTION(BUILD_GUI "Builds gui executable" ON)
OPTION(BUILD_SERVER "Builds server executable" ON)
OPTION(STATIC_WINDOWS_BUILD "Builds a static windows binary" ON)
OPTION(QT6_INSTALL_PREFIX "Qt install prefix" "")

if (NOT WIN32)
    set(STATIC_WINDOWS_BUILD OFF)
endif()

if (STATIC_WINDOWS_BUILD)
    message("Building QT as static library")

    if(QT6_INSTALL_PREFIX STREQUAL "" OR NOT QT6_INSTALL_PREFIX)
        set(QT6_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/qt6-install" CACHE PATH "Qt6 install prefix" FORCE)
    endif()
    set(QT_BUILD_PATH ${CMAKE_BINARY_DIR}/qt-static)

    execute_process(COMMAND
        ${CMAKE_COMMAND}
        -S ${CMAKE_SOURCE_DIR}/cmake/qt
        -B ${QT_BUILD_PATH}
        -DQT6_INSTALL_PREFIX=${QT6_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -GNinja
        RESULT_VARIABLE command_result
    )

    if (command_result)
        message(FATAL_ERROR "Qt bootstrap configure failed")
    endif()

    execute_process(COMMAND
        ${CMAKE_COMMAND}
        --build ${QT_BUILD_PATH}
        --parallel
        RESULT_VARIABLE command_result
    )

    if (command_result)
        message(FATAL_ERROR "Qt bootstrap build failed")
    endif()

    execute_process(COMMAND
        ${CMAKE_COMMAND}
        --install ${QT_BUILD_PATH}
        RESULT_VARIABLE command_result
    )

    if (command_result)
        message(FATAL_ERROR "Qt bootstrap install failed")
    endif()

    list(PREPEND CMAKE_PREFIX_PATH ${QT6_INSTALL_PREFIX})
endif()

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
