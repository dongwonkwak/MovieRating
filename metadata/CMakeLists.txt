cmake_minimum_required(VERSION 3.20.0)
project(metadata)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../)

add_executable(metadata 
    main.cpp 
    controller/controller.cpp
    repository/repository.cpp
    service/metadataservice.cpp
)

target_link_libraries(metadata PRIVATE 
    cpprestsdk::cpprest 
    ${OPENSSL_LIBRARIES} 
    common)