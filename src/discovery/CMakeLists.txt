cmake_minimum_required(VERSION 3.20.0)
project(discovery)

find_package(ppconsul CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../)

add_library(discovery consul.cpp memory.cpp registry.cpp)

target_link_libraries(discovery PRIVATE
    fmt::fmt
    range-v3
    common
    config
    ppconsul)

target_compile_options(discovery PUBLIC -Wall -Werror)