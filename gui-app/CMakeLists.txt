cmake_minimum_required(VERSION 3.12)

project(gui-app)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(FETCHCONTENT_QUIET OFF)
include(FetchContent)
FetchContent_Declare(
    libharu
    GIT_REPOSITORY https://github.com/libharu/libharu
    GIT_TAG v2.4.5
)

FetchContent_MakeAvailable(libharu)
if(NOT TARGET hpdf::hpdf)
    add_library(hpdf::hpdf ALIAS hpdf)
endif()

qt_standard_project_setup()

add_library(gui_lib STATIC ${SRCFILES})
target_link_libraries(gui_lib
    PUBLIC
    Qt6::Widgets
    generated_protobufs
    sane_in_the_membrane_utils_lib
    hpdf::hpdf
)

add_executable(Gui src/main.cpp)

target_link_libraries(Gui PRIVATE gui_lib)
