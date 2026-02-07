cmake_minimum_required(VERSION 4.2)

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

set(CMAKE_POLICY_VERSION_MINIMUM "3.5")
FetchContent_Declare(
    mdns
    GIT_REPOSITORY https://github.com/mjansson/mdns
    GIT_TAG 1.4.3
)
FetchContent_MakeAvailable(mdns)
if(NOT TARGET mdns::mdns)
    add_library(mdns::mdns ALIAS mdns)
endif()

qt_standard_project_setup()

add_library(gui_lib STATIC ${SRCFILES})
target_link_libraries(gui_lib
    PUBLIC
    Qt6::Widgets
    generated_protobufs
    sane_in_the_membrane_utils_lib
    hpdf::hpdf
    mdns::mdns
)

add_executable(Gui src/main.cpp)

target_link_libraries(Gui PRIVATE gui_lib)

target_compile_options(Gui PRIVATE ${COMPILER_WARNING_FLAGS})
