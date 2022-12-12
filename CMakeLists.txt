cmake_minimum_required(VERSION 3.20.0)
project(MovieRating)

set(CMAKE_CXX_STANDARD 20)

find_package(cpprestsdk CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)


add_subdirectory(common)
add_subdirectory(discovery)
add_subdirectory(metadata)
add_subdirectory(movie)
add_subdirectory(rating)