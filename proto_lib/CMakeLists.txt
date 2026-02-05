cmake_minimum_required(VERSION 3.12)

project(generated_protobufs C CXX)

if (CMAKE_BUILD_TYPE MATCHES Debug AND NOT BUILD_STATIC)
    find_package(gRPC CONFIG REQUIRED)
    find_package(Protobuf REQUIRED)
else()
    set(protobuf_INSTALL OFF CACHE BOOL "Disable protobuf installation" FORCE)
    set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "Disable abseil installation" FORCE)
    set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    include(FetchContent)
    FetchContent_Declare(
        gRPC
        GIT_REPOSITORY https://github.com/grpc/grpc
        GIT_TAG        v1.78.0
    )
    set(FETCHCONTENT_QUIET OFF)

    set(gRPC_BUILD_TESTS OFF)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
    FetchContent_MakeAvailable(gRPC)
    if(NOT TARGET gRPC::grpc++)
        add_library(gRPC::grpc++ ALIAS grpc++)
    endif()
endif()

file(GLOB_RECURSE PROTO_FILES
    "../proto/*.proto"
    "../proto/**/*.proto"
)

if(NOT PROTO_FILES)
    message(FATAL_ERROR "No .proto files found in proto/ folder!")
endif()

if (NOT (CMAKE_BUILD_TYPE MATCHES Debug AND NOT BUILD_STATIC))
    include(${grpc_SOURCE_DIR}/third_party/protobuf/cmake/protobuf-generate.cmake)
endif()

add_library(generated_protobufs OBJECT ${PROTO_FILES})

target_link_libraries(generated_protobufs PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
)

set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(PROTO_IMPORT_DIRS "../proto")

target_include_directories(generated_protobufs PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

protobuf_generate(
    TARGET generated_protobufs
    IMPORT_DIRS ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")

if (CMAKE_BUILD_TYPE MATCHES Debug AND NOT BUILD_STATIC)
    protobuf_generate(
        TARGET generated_protobufs
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
        IMPORT_DIRS ${PROTO_IMPORT_DIRS}
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")
else()
    protobuf_generate(
        TARGET generated_protobufs
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:grpc_cpp_plugin>"
        IMPORT_DIRS ${PROTO_IMPORT_DIRS}
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")
endif()
