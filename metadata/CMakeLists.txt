cmake_minimum_required(VERSION 3.20.0)
project(metadata)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../)
include_directories(${CMAKE_BINARY_DIR})

find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
find_package(cppcoro CONFIG REQUIRED)

add_executable(metadata 
    main.cpp 
    controller/controller.cpp
    repository/repository.cpp
    service/http/metadata_service.cpp
    service/grpc/metadata_service.cpp
)

target_include_directories(metadata PRIVATE ${RAPIDCSV_INCLUDE_DIRS})
target_link_libraries(metadata PRIVATE 
    cpprestsdk::cpprest 
    ${OPENSSL_LIBRARIES}
    discovery
    cppcoro
    proto-object
    common)