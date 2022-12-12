#include "movie/gateway/metadata.h"
#include "movie/gateway/rating.h"
#include "movie/controller/controller.h"
#include "movie/service/movieservice.h"

#include <iostream>
#include <memory>
#include <string>

#include <discovery/consul.h>
#include <jthread.hpp>
#include <condition_variable_any2.hpp>

using namespace movie;

int main(int argc, char* argv[])
{
    std::string serviceName = "movie";
    auto serviceId = discovery::GenerateServiceID(serviceName);
    auto registry = discovery::ConsulRegistry::Create();
    registry->Register(serviceId, serviceName, "8083");
    
    auto t = std::jthread([&registry, &serviceId](const std::stop_token& token) {
        bool isStart = true;
        std::mutex mutex;

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

    // create meta gateway
    auto metadataGateway = std::make_unique<MetadataGateway>(registry);
    auto ratingGateway = std::make_unique<RatingGateway>(registry);
    auto controller = std::make_unique<Controller>(
        std::move(ratingGateway),
        std::move(metadataGateway));
    const string_t addr = "http://localhost:8083/movie";
    auto service = std::make_unique<MovieService>(std::move(controller), addr);

    ucout << utility::string_t(U("Movie Service Listening for requests at: ")) << addr << std::endl;
    service->start().wait();

    std::string line;
    std::getline(std::cin, line);

    service->stop().then([&]{
        t.request_stop();
    }).wait();

    return 0;
}