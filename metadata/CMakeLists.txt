cmake_minimum_required(VERSION 3.20.0)
project(metadata)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../)
include_directories(${CMAKE_BINARY_DIR})

find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")

add_executable(metadata 
    main.cpp 
    controller/controller.cpp
    repository/repository.cpp
    service/metadataservice.cpp
)

target_include_directories(metadata PRIVATE ${RAPIDCSV_INCLUDE_DIRS})
target_link_libraries(metadata PRIVATE 
    cpprestsdk::cpprest 
    ${OPENSSL_LIBRARIES}
    discovery
    proto-object
    common)