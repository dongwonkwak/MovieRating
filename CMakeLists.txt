cmake_minimum_required(VERSION 3.20.0)
project(MovieRating)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

include(api/grpc_protobuf_generate.cmake)

add_subdirectory(common)
add_subdirectory(discovery)
add_subdirectory(metadata)
add_subdirectory(movie)
add_subdirectory(rating)