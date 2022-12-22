cmake_minimum_required(VERSION 3.20.0)
project(movie)

include_directories(../)
include_directories(${CMAKE_BINARY_DIR})

find_package(cppcoro CONFIG REQUIRED)

add_executable(movie 
    main.cpp
    service/http/movie_service.cpp
    service/grpc/movie_service.cpp
    controller/controller.cpp
    ../rating/model/rating.cpp
    gateway/rating/grpc/rating.cpp
    gateway/rating/http/rating.cpp
    gateway/metadata/grpc/metadata.cpp
    gateway/metadata/http/metadata.cpp
    grpcutil/grpcutil.cpp
)

target_include_directories(movie PRIVATE ../metadata)
target_link_libraries(movie PRIVATE 
    cpprestsdk::cpprest 
    ${OPENSSL_LIBRARIES} 
    discovery
    cppcoro
    proto-object
    common)