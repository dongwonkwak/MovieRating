#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "test_application.h"
#include "test_utils.h"

#include "discovery/consul.h"
#include "rating/model/rating.h"
#include "movie/gateway/rating/grpc/rating.h"
#include "rating/service/grpc/rating_service.h"
#include "rating/controller/controller.h"
#include "rating/ingester/kafka/ingester.h"
// service put data into database

TEST(ServiceTest, Run)
{
    TestApplication app;
    auto controller = app()->resolve<rating::controller::Controller>();
    auto service = std::make_shared<rating::service::grpc::RatingService>(controller, RATING_GRPC_ENDPOINT);
    ServiceRunner runner(service);
    auto registry = app()->resolve<discovery::Registry>();
    auto serviceId = registry->GetServiceID();
    registry->ReportHealthyState(serviceId);

    // send dummy data to server
    auto client = std::make_shared<movie::gateway::rating::grpc::RatingGateway>(registry);
    rating::model::Rating rating;
    const std::string recordID = "1";
    const std::string recordType = "1";
    rating.recordId = recordID;
    rating.ratingValue = 5;
    rating.userId = "105";
    // client -> service
    client->PutRating(recordID, recordType, rating);

    auto results = controller->Get(recordID, recordType);
    EXPECT_TRUE(results.has_value());
    auto& v = results.value();
    spdlog::info("size: {}", v.size());
    EXPECT_TRUE(v.size() == 1);
    auto& item = *v.begin();
    EXPECT_EQ(item.ratingValue, rating.ratingValue);
    EXPECT_EQ(item.recordId, recordID);
    EXPECT_EQ(item.recordType, recordType);
    EXPECT_EQ(item.userId, rating.userId);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    runner.try_join();
}