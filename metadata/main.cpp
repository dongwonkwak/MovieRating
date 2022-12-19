#include "metadata/repository/repository.h"
#include "metadata/controller/controller.h"
#include "metadata/service/http/metadata_service.h"
#include "metadata/service/grpc/metadata_service.h"
using namespace metadata;

#include <rapidcsv.h>

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


int main(int argc, char* argv[])
{
    auto repository = std::make_unique<repository::Repository>();
    rapidcsv::Document doc("./data/metadata.csv");
    auto count = doc.GetRowCount();
    for (size_t i = 0; i < count; i++)
    {
        model::Metadata data;
        auto row = doc.GetRow<std::string>(i);
        data.id = row[0];
        data.title = row[1];
        data.director = row[2];
        data.description = row[3];
        repository->Put(data.id, data);
    }
    std::string serviceName = "metadata";
    auto serviceId = discovery::GenerateServiceID(serviceName);

    auto registry = discovery::ConsulRegistry::Create();
    registry->Register(serviceId, serviceName, "8081");

    //std::mutex mutex;
    /*auto t = std::jthread([&](const std::stop_token& token) {
        bool isStart = true;
        
        while (isStart)
        {
            registry->ReportHealthyState(serviceId);
            std::unique_lock lock(mutex);
            isStart = !std::condition_variable_any2().wait_for(
                lock, token, std::chrono::seconds(3),
                [&token] { return token.stop_requested();});
            std::cout << "ping\n";
        }
    });*/


    auto controller = std::make_unique<controller::Controller>(std::move(repository));
    const string_t addr = "localhost:8081";
    auto service = std::make_unique<service::grpc::MetadataService>(std::move(controller), addr);
    ucout << utility::string_t(U("Metadata Service Listening for requests at: ")) << addr << std::endl;
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