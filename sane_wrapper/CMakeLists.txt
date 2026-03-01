cmake_minimum_required(VERSION 4.2)

project(sane_in_the_membrane)

file(GLOB_RECURSE SRCFILES "src/*.cpp")
file(GLOB_RECURSE PUBLIC_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

add_library(sane_in_the_membrane_lib STATIC ${PUBLIC_HEADERS} ${SRCFILES})
target_include_directories(sane_in_the_membrane_lib
    PUBLIC "./include"
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SANE REQUIRED IMPORTED_TARGET sane-backends)

target_link_libraries(sane_in_the_membrane_lib
    PRIVATE
        PkgConfig::SANE
)

target_compile_options(sane_in_the_membrane_lib PRIVATE ${COMPILER_WARNING_FLAGS})
target_compile_features(sane_in_the_membrane_lib PUBLIC cxx_std_23)

if (BUILD_TESTS)
    file(GLOB_RECURSE MOCK_IMPL "mock/*.cpp")

    add_library(sane_in_the_membrane_test_lib STATIC 
        ${PUBLIC_HEADERS} 
        ${SRCFILES} 
        ${MOCK_IMPL}
    )
    target_include_directories(sane_in_the_membrane_test_lib PRIVATE ${SANE_INCLUDE_DIRS})
    target_include_directories(sane_in_the_membrane_test_lib
        PUBLIC "./include"
        PUBLIC "./mock"
    )

    target_compile_options(sane_in_the_membrane_test_lib PRIVATE ${COMPILER_WARNING_FLAGS})
    target_compile_features(sane_in_the_membrane_test_lib PUBLIC cxx_std_23)
endif()
