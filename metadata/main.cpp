#include <iostream>
#include "repository/repository.h"
#include "controller/controller.h"
#include "service/metadataservice.h"
using namespace metadata;

#include <rapidcsv.h>

#include <discovery/consul.h>
#include <jthread.hpp>
#include <condition_variable_any2.hpp>


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
    std::mutex mutex;
    auto t = std::jthread([&](const std::stop_token& token) {
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
    });


    auto controller = std::make_unique<controller::Controller>(std::move(repository));
    const string_t addr = "http://localhost:8081/metadata";
    auto service = std::make_unique<service::MetadataService>(std::move(controller), addr);
    ucout << utility::string_t(U("Metadata Service Listening for requests at: ")) << addr << std::endl;
    service->start().wait();

    std::string line;
    std::getline(std::cin, line);

    service->stop().then([&t] {
        t.request_stop();
    }).wait();

    return 0;
}