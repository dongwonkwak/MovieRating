cmake_minimum_required(VERSION 3.20.0)
project(rating)

include_directories(../)
include_directories(../extern/include)
include_directories(${CMAKE_BINARY_DIR})

find_package(cppcoro CONFIG REQUIRED)
find_package(RdKafka CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

add_executable(rating 
    main.cpp
    repository/memory/memory.cpp
    repository/postgresql/postgresql.cpp
    service/grpc/rating_service.cpp
    service/http/rating_service.cpp
    controller/controller.cpp
    ingester/kafka/ingester.cpp
)

target_link_libraries(rating PRIVATE 
    cpprestsdk::cpprest 
    ${OPENSSL_LIBRARIES}
    RdKafka::rdkafka RdKafka::rdkafka++
    libpqxx::pqxx
    discovery
    cppcoro
    proto-object
    common)

add_subdirectory(test)