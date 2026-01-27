cmake_minimum_required(VERSION 3.12)

project(gui-app)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

add_library(gui_lib STATIC ${SRCFILES})
target_link_libraries(gui_lib
    PUBLIC
    Qt6::Widgets
    generated_protobufs
    sane_in_the_membrane_utils_lib
)

add_executable(Gui src/main.cpp)

target_link_libraries(Gui PRIVATE gui_lib)
