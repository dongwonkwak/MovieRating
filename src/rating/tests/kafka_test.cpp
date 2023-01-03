#include <gtest/gtest.h>
#include "rating/ingester/kafka/ingester.h"

using namespace rating::ingester::kafka;

static auto make_ingester()
{
    return Ingester::NewIngester(
        KAFKA_TEST_INSTANCE,
        KAFKA_GROUP_ID,
        KAFKA_TOPIC
    );    
}

TEST(create_ingester_instance, consumer)
{
    auto ingester = make_ingester();
    EXPECT_TRUE(ingester != nullptr);
}
