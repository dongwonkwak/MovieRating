cmake_minimum_required(VERSION 3.20.0)
project(common)

find_package(tl-expected CONFIG REQUIRED)

add_library(common INTERFACE)

target_include_directories(common INTERFACE .)
target_link_libraries(common INTERFACE tl::expected)