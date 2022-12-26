#include "spdlog/spdlog.h"

#include "movie/gateway/metadata/http/metadata.h"
#include "movie/gateway/rating/http/rating.h"
#include "movie/gateway/metadata/grpc/metadata.h"
#include "movie/gateway/rating/grpc/rating.h"
#include "movie/controller/controller.h"
#include "movie/service/http/movie_service.h"
#include "movie/service/grpc/movie_service.h"
using namespace movie;

#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/on_scope_exit.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/schedule_on.hpp>

#include <discovery/consul.h>
#include <jthread.hpp>
#include <condition_variable_any2.hpp>

#include <iostream>
#include <memory>
#include <string>


int main(int argc, char* argv[])
{
    std::string serviceName = "movie";
    auto serviceId = discovery::GenerateServiceID(serviceName);
    auto registry = discovery::ConsulRegistry::Create();
    registry->Register(serviceId, serviceName, "8083");

    // create meta gateway
    auto metadataGateway = std::make_unique<gateway::metadata::grpc::MetadataGateway>(registry);
    auto ratingGateway = std::make_unique<gateway::rating::grpc::RatingGateway>(registry);
    auto controller = std::make_unique<controller::Controller>(
        std::move(ratingGateway),
        std::move(metadataGateway));
    const string_t addr = "localhost:8083";
    auto service = std::make_unique<movie::service::grpc::MovieService>(std::move(controller), addr);
    spdlog::info("Movie Service Listening for requests at: {}", addr.c_str());
    auto thread_pool = cppcoro::static_thread_pool{2};

    auto func = [&]() -> cppcoro::task<>
    {
        service->start();
        co_return;
    };

    auto health_check = [&]() -> cppcoro::task<>
    {
        co_await thread_pool.schedule();
        registry->ReportHealthyState(serviceId);

        std::this_thread::sleep_for(std::chrono::seconds(3));
        co_return;
    };

    cppcoro::sync_wait(cppcoro::when_all(
        [&]() -> cppcoro::task<>
        {
            auto stopOnExit = cppcoro::on_scope_exit([&] { registry->Deregister(serviceId); });
            while (true)
            {
                co_await cppcoro::schedule_on(thread_pool, health_check());
                std::cout << "ping\n";
            }
        }(),
        [&]() -> cppcoro::task<>
        {
            auto stopOnExit = cppcoro::on_scope_exit([&] { service->stop(); });
            co_await func();
        }()
    ));

    return 0;
}