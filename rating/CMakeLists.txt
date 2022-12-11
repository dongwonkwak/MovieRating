cmake_minimum_required(VERSION 3.20.0)
project(rating)

find_package(cpprestsdk REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../)

add_executable(rating 
    main.cpp
    repository/memory.cpp
    model/rating.cpp
    service/ratingservice.cpp
    controller/controller.cpp
)

target_link_libraries(rating PRIVATE cpprestsdk::cpprest ${OPENSSL_LIBRARIES} tl::expected)