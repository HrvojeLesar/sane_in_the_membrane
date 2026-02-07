cmake_minimum_required(VERSION 4.2)

project(sane_in_the_membrane_utils C CXX)

file(GLOB_RECURSE SRCFILES "src/*.cpp")
file(GLOB_RECURSE PUBLIC_HEADERS CONFIGURE_DEPENDS "src/*.hpp")

add_library(sane_in_the_membrane_utils_lib STATIC ${PUBLIC_HEADERS} ${SRCFILES})

target_include_directories(sane_in_the_membrane_utils_lib
    PUBLIC
    "src"
)

set_target_properties(sane_in_the_membrane_utils_lib PROPERTIES LINKER_LANGUAGE CXX)
target_compile_options(sane_in_the_membrane_utils_lib PRIVATE ${COMPILER_WARNING_FLAGS})
