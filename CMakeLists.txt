cmake_minimum_required(VERSION 3.20.0)
project(MovieRating)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

include(api/grpc_protobuf_generate.cmake)


add_subdirectory(src/common)
add_subdirectory(src/config)
add_subdirectory(src/discovery)
add_subdirectory(src/metadata)
add_subdirectory(src/movie)
add_subdirectory(src/rating)