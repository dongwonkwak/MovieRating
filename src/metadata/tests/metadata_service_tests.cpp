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
    auto serviceName = "metadata";
    TestApplication app;
    auto controller = app()->resolve<metadata::controller::Controller>();
    std::string endpoint(METADATA_GRPC_ENDPOINT);
    auto service = std::make_shared<metadata::service::grpc::MetadataService>(controller, endpoint);
    ServiceRunner runner(service);

    auto registry = app()->resolve<discovery::Registry>();
    auto serviceId = discovery::GenerateServiceID(serviceName);
    auto pos = endpoint.find(':');
    if (pos == std::string::npos)
    {
        std::cerr << "can't find port\n";
        return;
    }
    auto port = endpoint.substr(pos + 1, endpoint.size());
    registry->Register(serviceId, serviceName, port);
    registry->ReportHealthyState(serviceId);

    auto client = std::make_shared<movie::gateway::metadata::grpc::MetadataGateway>(registry);
    auto testID = "500";
    auto result = client->Get(testID);
    EXPECT_FALSE(result.has_value());

    std::this_thread::sleep_for(std::chrono::seconds(3));
    runner.try_join();
}