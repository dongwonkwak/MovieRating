cmake_minimum_required(VERSION 3.20.0)
set(project_name movie)
set(CMAKE_CXX_STANDARD 20)

project(${project_name} LANGUAGES CXX)

include_directories(../)
include_directories(${CMAKE_BINARY_DIR})

add_library(${project_name}-lib 
    application.cpp
    service/http/movie_service.cpp
    service/grpc/movie_service.cpp
    controller/controller.cpp
    gateway/rating/grpc/rating.cpp
    gateway/rating/http/rating.cpp
    gateway/metadata/grpc/metadata.cpp
    gateway/metadata/http/metadata.cpp
    grpcutil/grpcutil.cpp
)

find_package(cppcoro CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
find_package(Boost 1.81.0 COMPONENTS program_options)
find_package(spdlog CONFIG REQUIRED)
find_path(HYPODERMIC_INCLUDE_DIRS "Hypodermic/ActivatedRegistrationInfo.h")

target_link_libraries(${project_name}-lib PRIVATE 
    cpprestsdk::cpprest 
    config
    spdlog::spdlog
    ${Boost_LIBRARIES}
    discovery
    cppcoro
    proto-object
    common)
target_include_directories(${project_name}-lib PRIVATE ${HYPODERMIC_INCLUDE_DIRS})

add_executable(${project_name} runner.cpp main.cpp)

target_link_libraries(${project_name} ${project_name}-lib fmt::fmt spdlog::spdlog)
add_dependencies(${project_name} ${project_name}-lib)