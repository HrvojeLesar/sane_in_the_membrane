cmake_minimum_required(VERSION 3.12)

project(gui-app)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

## TODO: Change this to be portable
find_path(LIBHARU_INCLUDE_DIR hpdf.h
  PATHS /usr/include
)
find_library(LIBHARU_LIBRARY NAMES libhpdf.so
  PATHS /usr/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libharu
    REQUIRED_VARS LIBHARU_LIBRARY LIBHARU_INCLUDE_DIR
)

if(LIBHARU_FOUND AND NOT TARGET libharu::libharu)
    add_library(libharu UNKNOWN IMPORTED)
    set_target_properties(libharu PROPERTIES
        IMPORTED_LOCATION "${LIBHARU_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBHARU_INCLUDE_DIR}"
      )
endif()

qt_standard_project_setup()

add_library(gui_lib STATIC ${SRCFILES})
target_link_libraries(gui_lib
    PUBLIC
    Qt6::Widgets
    generated_protobufs
    sane_in_the_membrane_utils_lib
    libharu
)

add_executable(Gui src/main.cpp)

target_link_libraries(Gui PRIVATE gui_lib)
