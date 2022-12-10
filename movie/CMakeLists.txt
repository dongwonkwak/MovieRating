cmake_minimum_required(VERSION 3.20.0)
project(movie)

find_package(cpprestsdk REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../)
#target_include_directories(movie PRIVATE ../metadata)
add_executable(movie 
    main.cpp
    service/movieservice.cpp
    controller/controller.cpp
    ../rating/model/rating.cpp
    gateway/metadata.cpp
    gateway/rating.cpp
)

target_include_directories(movie PRIVATE ../metadata)
target_link_libraries(movie PRIVATE cpprestsdk::cpprest ${OPENSSL_LIBRARIES} expected)