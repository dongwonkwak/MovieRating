#include <gtest/gtest.h>
#include "test_application.h"

#include "metadata/controller/controller.h"
#include "metadata/service/grpc/metadata_service.h"
#include "metadata/repository/repository.h"
#include "discovery/consul.h"
#include "movie/gateway/metadata/grpc/metadata.h"

#include <jthread.hpp>

template <typename Service>
class ServiceRunner
{
public:
    ServiceRunner(Service& service)
        : service_(service)
    {
        thread_ = std::jthread([&]{
            service_->start();
        });
    }

    void try_join()
    {
        service_->stop();
        if (thread_.joinable())
        {
            thread_.join();
        }
    }
private:
    Service& service_;
    std::jthread thread_;
};

TEST(METAServiceTest, Run)
{
    TestApplication app;
    auto controller = app()->resolve<metadata::controller::Controller>();
    auto service = std::make_shared<metadata::service::grpc::MetadataService>(controller, METADATA_GRPC_ENDPOINT);
    ServiceRunner runner(service);

    auto registry = app()->resolve<discovery::Registry>();
    auto serviceId = registry->GetServiceID();
    registry->ReportHealthyState(serviceId);

    auto client = std::make_shared<movie::gateway::metadata::grpc::MetadataGateway>(registry);
    auto testID = "500";
    auto result = client->Get(testID);
    EXPECT_FALSE(result.has_value());

    std::this_thread::sleep_for(std::chrono::seconds(3));
    runner.try_join();
}