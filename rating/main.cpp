#include <iostream>
#include "repository/memory.h"
#include "controller/controller.h"
#include "service/ratingservice.h"
using namespace rating;

#include <discovery/consul.h>
#include <jthread.hpp>
#include <condition_variable_any2.hpp>

int main(int argc, char* argv[])
{
    std::string serviceName = "rating";
    auto serviceId = discovery::GenerateServiceID(serviceName);

    auto registry = discovery::ConsulRegistry::Create();
    registry->Register(serviceId, serviceName, "8082");
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

    auto repository = std::make_unique<Repository>();
    auto controller = std::make_unique<Controller>(std::move(repository));
    const string_t addr = "http://localhost:8082/rating";
    auto service = std::make_unique<RatingService>(std::move(controller), addr);
    ucout << utility::string_t(U("Rating Service Listening for requests at: ")) << addr << std::endl;

    service->start().wait();

    std::string line;
    std::getline(std::cin, line);

    service->stop().then([&]{
        t.request_stop();
    }).wait();

    return 0;
}