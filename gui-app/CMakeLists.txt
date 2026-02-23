cmake_minimum_required(VERSION 4.2)

project(gui-app)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)

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
target_include_directories(hpdf INTERFACE
    "${libharu_SOURCE_DIR}/include"
    "${libharu_BINARY_DIR}/include"
)

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
    Qt6::Svg
    generated_protobufs
    sane_in_the_membrane_utils_lib
    hpdf::hpdf
    mdns::mdns
)
target_compile_definitions(gui_lib PRIVATE
    NOMINMAX
)
target_compile_features(gui_lib PUBLIC cxx_std_23)

if (NOT WIN32)
    add_executable(Gui src/main.cpp)
else()
    add_executable(Gui WIN32 src/main.cpp)
endif()

target_link_libraries(Gui PRIVATE gui_lib)

if (STATIC_WINDOWS_BUILD)
    target_link_libraries(Gui PRIVATE Qt6::QWindowsIntegrationPlugin)
    target_compile_definitions(Gui PRIVATE WINDOWS_QT_PLUGINS)
endif()

qt_add_resources(gui_lib "custom-icons"
    PREFIX "/"
    FILES
        assets/icons/arrow-clockwise.svg
        assets/icons/arrow-counterclockwise.svg
        assets/icons/arrow-left.svg
        assets/icons/arrow-right.svg
        assets/icons/symmetry-vertical.svg
        assets/icons/rubbish-bin.svg
)
