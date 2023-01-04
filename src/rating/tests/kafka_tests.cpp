#include <gtest/gtest.h>
#include <cppkafka/cppkafka.h>
using namespace cppkafka;

#include "rating/ingester/kafka/ingester.h"
using namespace rating::ingester::kafka;

#include "rating/model/rating.h"
using namespace rating::model;

#include "test_utils.h"

#include <string>
#include <vector>

auto ratingsdata = R"([{"userId":"105","recordId":"1","recordType":"1","value":5,"providerId":"test-provider","eventType":"put"},{"userId":"105","recordId":"2","recordType":"1","value":4,"providerId":"test-provider","eventType":"put"}]
)";

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

TEST(message_consume, consumer)
{
    std::vector<RatingEvent> events;
    auto parsed = parseRatings(ratingsdata);

    EXPECT_TRUE(parsed.size() == 2);

    Configuration config = {
        { "metadata.broker.list", KAFKA_TEST_INSTANCE }
    };
    Producer producer(config);
    for (const auto& item : parsed)
    {
        auto message = item.AsJSON().serialize();
        producer.produce(MessageBuilder(KAFKA_TOPIC).partition(KAFKA_NUM_PARTITIONS-1).payload(message));
        producer.flush();
    }

    auto consumer = make_ingester();
    ConsumerRunner runner(consumer);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    runner.try_join();
    auto results = runner.get_events();
    EXPECT_TRUE(results.size() == 2);
}