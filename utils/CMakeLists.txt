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

find_package(ImageMagick REQUIRED COMPONENTS Magick++)
add_library(sane_in_the_membrane_utils_image_lib
    STATIC
    src/image/MagickImageWrapper.hpp
    src/image/MagickImageWrapper.cpp
)
target_include_directories(sane_in_the_membrane_utils_image_lib
    PUBLIC
    "src/image"
)


set_target_properties(sane_in_the_membrane_utils_lib PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(sane_in_the_membrane_utils_image_lib PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(sane_in_the_membrane_utils_image_lib
    PUBLIC
        ImageMagick::Magick++
)
