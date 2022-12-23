#include <iostream>
#include "rating/repository/memory/memory.h"
#include "rating/repository/postgresql/postgresql.h"
#include "rating/controller/controller.h"
#include "rating/service/http/rating_service.h"
#include "rating/service/grpc/rating_service.h"
#include "rating/ingester/kafka/ingester.h"
using namespace rating;

#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/on_scope_exit.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/schedule_on.hpp>

#include <discovery/consul.h>

int main(int argc, char* argv[])
{
    std::string serviceName = "rating";
    auto serviceId = discovery::GenerateServiceID(serviceName);

    auto registry = discovery::ConsulRegistry::Create();
    registry->Register(serviceId, serviceName, "8082");

    //auto repository = std::make_unique<repository::memory::Repository>();
    auto repository = std::make_unique<repository::postresql::Repository>("postgresql://postgres:postgres@localhost:5432/movie_db");
    auto ingester = ingester::kafka::Ingester::NewIngester("localhost", "movie-rating", "ratings");
    auto controller = std::make_unique<controller::Controller>(std::move(repository), ingester);
    const string_t addr = "localhost:8082";
    auto service = std::make_unique<service::grpc::RatingService>(std::move(controller), addr);
    ucout << utility::string_t(U("Rating Service Listening for requests at: ")) << addr << std::endl;

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