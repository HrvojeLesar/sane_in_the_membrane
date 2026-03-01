cmake_minimum_required(VERSION 4.2)

project(tests)

file(GLOB_RECURSE SRCFILES "src/*.cpp")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests ${SRCFILES})
target_link_libraries(tests
    sane_in_the_membrane_test_lib
    server_lib_test
    gui_lib
    sane_in_the_membrane_utils_lib
    GTest::gtest_main
)

# target_compile_options(tests PRIVATE --coverage)
# target_link_options(tests PRIVATE --coverage)

include(GoogleTest)
gtest_discover_tests(tests)
