cmake_minimum_required(VERSION 3.20.0)
set(project_name rating-service-test)
project(${project_name})

if (NOT KAFKA_TEST_INSTANCE)
set(KAFKA_TEST_INSTANCE localhost:9092,localhost:9093,localhost:9094
    CACHE STRING "The kafka instance to which to connect to run tests")
endif()

if (NOT KAFKA_NUM_PARTITIONS)
    set(KAFKA_NUM_PARTITIONS 3 CACHE STRING "Kafka Number of partitions")
endif()

if (NOT KAFKA_GROUP_ID)
    set(KAFKA_GROUP_ID "movie-rating" CACHE STRING "Kafka group id")
endif()

if (NOT KAFKA_TOPIC)
    set(KAFKA_TOPIC "ratings" CACHE STRING "Kafka topic")
endif()

if (NOT RATING_POSTGRESQL_CONNECTION_URL)
    set(RATING_POSTGRESQL_CONNECTION_URL "postgresql://postgres:postgres@localhost:5432/movie_db" CACHE STRING "postgresql connection string")
endif()

if (NOT RATING_CONSUL_ENDPOINT)
    set(RATING_CONSUL_ENDPOINT "http://127.0.0.1:8500" CACHE STRING "The consul instance where to connect to run tests")
endif()

if (NOT RATING_GRPC_ENDPOINT)
    set(RATING_GRPC_ENDPOINT "localhost:8082" CACHE STRING "Grpc service endpoint")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(
    "-DKAFKA_TEST_INSTANCE=\"${KAFKA_TEST_INSTANCE}\""
    -DKAFKA_NUM_PARTITIONS=${KAFKA_NUM_PATIRIONS}
    "-DKAFKA_TOPIC=\"${KAFKA_TOPIC}\""
    "-DKAFKA_GROUP_ID=\"${KAFKA_GROUP_ID}\""
    "-DRATING_POSTGRESQL_CONNECTION_URL=\"${RATING_POSTGRESQL_CONNECTION_URL}\""
    "-DRATING_CONSUL_ENDPOINT=\"${RATING_CONSUL_ENDPOINT}\""
    "-DRATING_GRPC_ENDPOINT=\"${RATING_GRPC_ENDPOINT}\""
)

set(SOURCES rating_service_tests.cpp test_utils.cpp test_application.cpp)

add_executable(${project_name} ${SOURCES})
include(Testing)
AddTests(${project_name})

target_link_libraries(${project_name} PRIVATE
    rating-lib
    movie-lib
)

set_target_properties(${project_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)