cmake_minimum_required(VERSION 3.20.0)
set(project_name metadata-service-test)
project(${project_name})


if (NOT MOVIE_RATING_CONSUL_ENDPOINT)
    set(MOVIE_RATING_CONSUL_ENDPOINT "http://127.0.0.1:8500" CACHE STRING "The consul instance where to connect to run tests")
endif()

if (NOT METADATA_GRPC_ENDPOINT)
    set(METADATA_GRPC_ENDPOINT "localhost:8081" CACHE STRING "Grpc service endpoint")
endif()

add_definitions(
    "-DMOVIE_RATING_CONSUL_ENDPOINT=\"${MOVIE_RATING_CONSUL_ENDPOINT}\""
    "-DMETADATA_GRPC_ENDPOINT=\"${METADATA_GRPC_ENDPOINT}\""
)

set(SOURCES metadata_service_tests.cpp test_application.cpp)

add_executable(${project_name} ${SOURCES})
target_include_directories(${project_name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
    ${CMAKE_CURRENT_SOURCE_DIR}
)

include(Testing)
AddTests(${project_name})

target_link_libraries(${project_name} PRIVATE
    metadata-lib
    movie-lib
)

set_target_properties(${project_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)