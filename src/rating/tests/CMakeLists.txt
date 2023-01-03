
if (NOT KAFKA_TEST_INSTANCE)
set(KAFKA_TEST_INSTANCE localhost:9092
    CACHE STRING "The kafka instance to which to connect to run tests")
endif()

if (NOT KAFKA_NUM_PARTITIONS)
    set(KAFKA_NUM_PATIRIONS 3 CACHE STRING "Kafka Number of partitions")
endif()

if (NOT KAFKA_GROUP_ID)
    set(KAFKA_GROUP_ID "movie-rating" CACHE STRING "Kafka group id")
endif()

if (NOT KAFKA_TOPIC)
    set(KAFKA_TOPIC "ratings" CACHE STRING "Kafka topic")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(
    "-DKAFKA_TEST_INSTANCE=\"${KAFKA_TEST_INSTANCE}\""
    -DKAFKA_NUM_PARTITIONS=${KAFKA_NUM_PATIRIONS}
    "-DKAFKA_TOPIC=\"${KAFKA_TOPIC}\""
    "-DKAFKA_GROUP_ID=\"${KAFKA_GROUP_ID}\""
)

add_executable(rating_tests
    kafka_test.cpp
)

target_link_libraries(rating_tests PUBLIC
    GTest::gtest GTest::gtest_main
    rating-lib
)