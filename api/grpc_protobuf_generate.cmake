
find_package(gRPC CONFIG REQUIRED)


find_program(_PROTOBUF_PROTOC protoc)
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

get_filename_component(movie_proto "api/movie.proto" ABSOLUTE)
get_filename_component(movie_proto_path "${movie_proto}" DIRECTORY)

set(movie_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/movie.pb.cc")
set(movie_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/movie.pb.h")
set(movie_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/movie.grpc.pb.cc")
set(movie_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/movie.grpc.pb.h")

add_custom_command(
    OUTPUT "${movie_proto_srcs}" "${movie_proto_hdrs}" "${movie_grpc_srcs}" "${movie_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}" 
     --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
     -I "${movie_proto_path}"
     --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
     "${movie_proto}"
    DEPENDS "${movie_proto}")

add_library(proto-object OBJECT
    ${movie_proto_srcs}
    ${movie_proto_hdrs}
    ${movie_grpc_srcs}
    ${movie_grpc_hdrs}
)


target_link_libraries(proto-object PUBLIC
    gRPC::grpc++
    gRPC::grpc++_reflection
)