cmake_minimum_required(VERSION 3.20.0)
project(config)

include_directories(../)

add_library(config config.cpp)

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(config PRIVATE
    yaml-cpp
)

target_compile_options(config PUBLIC -Wall -Werror)