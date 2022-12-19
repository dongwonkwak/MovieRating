cmake_minimum_required(VERSION 3.20.0)
project(rating)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../)
include_directories(${CMAKE_BINARY_DIR})

find_package(cppcoro CONFIG REQUIRED)

add_executable(rating 
    main.cpp
    repository/memory.cpp
    model/rating.cpp
    service/grpc/rating_service.cpp
    service/http/rating_service.cpp
    controller/controller.cpp
)

target_link_libraries(rating PRIVATE 
    cpprestsdk::cpprest 
    ${OPENSSL_LIBRARIES} 
    discovery
    cppcoro
    proto-object
    common)