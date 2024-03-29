cmake_minimum_required(VERSION 3.20.0)
set(project_name metadata)
set(CMAKE_CXX_STANDARD 20)

project(${project_name} LANGUAGES CXX)

add_library(${project_name}-lib 
    application.cpp
    controller/controller.cpp
    repository/memory/memory.cpp
    repository/postgresql/postgresql.cpp
    service/http/metadata_service.cpp
    service/grpc/metadata_service.cpp
)

find_package(cppcoro CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
find_package(Boost 1.81.0 COMPONENTS program_options)
find_package(spdlog CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_path(HYPODERMIC_INCLUDE_DIRS "Hypodermic/ActivatedRegistrationInfo.h")

target_link_libraries(${project_name}-lib PUBLIC 
    cpprestsdk::cpprest 
    config
    spdlog::spdlog
    ${Boost_LIBRARIES}
    libpqxx::pqxx
    discovery
    cppcoro
    proto-object
    common)

target_include_directories(${project_name}-lib PUBLIC 
    ${HYPODERMIC_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}
    ../
)

add_executable(${project_name} runner.cpp main.cpp)

target_link_libraries(${project_name} ${project_name}-lib fmt::fmt spdlog::spdlog)
add_dependencies(${project_name} ${project_name}-lib)

add_subdirectory(tests)