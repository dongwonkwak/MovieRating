cmake_minimum_required(VERSION 3.20.0)
project(test_kafka)

find_package(cpprestsdk CONFIG REQUIRED)
find_package(RdKafka CONFIG REQUIRED)

include_directories(../../extern/include)
include_directories(../../)

add_executable(test_kafka test_kafka.cpp)

target_link_libraries(test_kafka PRIVATE 
    cpprestsdk::cpprest 
    ${OPENSSL_LIBRARIES}
    RdKafka::rdkafka RdKafka::rdkafka++
)