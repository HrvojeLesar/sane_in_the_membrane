cmake_minimum_required(VERSION 3.12)

project(generated_protobufs C CXX)

find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf REQUIRED)

file(GLOB_RECURSE PROTO_FILES
    "../proto/*.proto"
    "../proto/**/*.proto"
)

if(NOT PROTO_FILES)
    message(FATAL_ERROR "No .proto files found in proto/ folder!")
endif()

# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

function(grpc_generate_cpp SRCS HDRS)
    set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../proto")
    set(${SRCS})
    set(${HDRS})
    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        file(RELATIVE_PATH REL_FIL ${PROTO_DIR} ${ABS_FIL})
        get_filename_component(FIL_WE ${FIL} NAME_WE)
        string(REPLACE ".proto" ".grpc.pb.cc" GRPC_CC ${REL_FIL})
        string(REPLACE ".proto" ".grpc.pb.h" GRPC_H ${REL_FIL})
        string(REPLACE ".proto" ".pb.cc" PB_CC ${REL_FIL})
        string(REPLACE ".proto" ".pb.h" PB_H ${REL_FIL})
        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${GRPC_CC}")
        list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${GRPC_H}")
        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${PB_CC}")
        list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${PB_H}")
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${GRPC_CC}"
                   "${CMAKE_CURRENT_BINARY_DIR}/${GRPC_H}"
                   "${CMAKE_CURRENT_BINARY_DIR}/${PB_CC}"
                   "${CMAKE_CURRENT_BINARY_DIR}/${PB_H}"
            COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
                 --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
                 --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
                 -I ${PROTO_DIR} ${REL_FIL}
            DEPENDS ${ABS_FIL}
            WORKING_DIRECTORY ${PROTO_DIR}
            COMMENT "Generating gRPC C++ code for ${FIL}"
            VERBATIM
        )
    endforeach()
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_FILES})

add_library(generated_protobufs STATIC
    ${GRPC_SRCS}
    ${GRPC_HDRS}
)

target_link_libraries(generated_protobufs PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
)

target_include_directories(generated_protobufs PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../proto
)
