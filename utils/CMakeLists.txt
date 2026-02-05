cmake_minimum_required(VERSION 3.12)

project(sane_in_the_membrane_utils C CXX)

add_library(sane_in_the_membrane_utils_lib
    STATIC
    src/SynchronizedAccess.hpp
)
target_include_directories(sane_in_the_membrane_utils_lib
    PUBLIC
    "src"
)

set_target_properties(sane_in_the_membrane_utils_lib PROPERTIES LINKER_LANGUAGE CXX)
