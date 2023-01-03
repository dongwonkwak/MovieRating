cmake_minimum_required(VERSION 3.20.0)
set(project_name rating)
set(CMAKE_CXX_STANDARD 20)

project(${project_name} LANGUAGES CXX)

include_directories(../)
include_directories(${CMAKE_BINARY_DIR})

add_library(${project_name}-lib  
    application.cpp
    repository/memory/memory.cpp
    repository/postgresql/postgresql.cpp
    service/grpc/rating_service.cpp
    service/http/rating_service.cpp
    controller/controller.cpp
    ingester/kafka/ingester.cpp
)

find_package(cppcoro CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
find_package(Boost 1.81.0 COMPONENTS program_options)
find_package(spdlog CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

target_link_libraries(${project_name}-lib PRIVATE 
    cpprestsdk::cpprest 
    config
    spdlog::spdlog
    ${Boost_LIBRARIES}
    ${CPPKAFKA_LIBRARY} RdKafka::rdkafka++
    libpqxx::pqxx
    discovery
    cppcoro
    proto-object
    common)
target_include_directories(${project_name}-lib PRIVATE 
    ${HYPODERMIC_INCLUDE_DIRS}
    ${CPPKAFKA_INCLUDE_DIR}
)

add_executable(${project_name} runner.cpp main.cpp)

target_link_libraries(${project_name} ${project_name}-lib fmt::fmt spdlog::spdlog)
add_dependencies(${project_name} ${project_name}-lib)

find_package(GTest REQUIRED)
enable_testing()
add_subdirectory(tests)