#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "movie/Application.h"
#include "movie/gateway/metadata/grpc/metadata.h"
#include "movie/gateway/rating/grpc/rating.h"


namespace movie
{
    using namespace config;

    Application::Application(const boost::program_options::variables_map& vm)
        : vm_{vm}
    {
        std::string configFile = vm["config-file"].as<std::string>();

        CREATE_SHARED_SERVICE(std::shared_ptr<config::Config>, config)([=]{
            return config::Config::Create(configFile);
        }());

        configService_ = config;

        CreateRegistry();   
        CreateController();
    }

    void Application::CreateRegistry()
    {
        CREATE_SHARED_SERVICE(std::shared_ptr<discovery::Registry>, registry)([]{
            GET_SERVICE(std::shared_ptr<config::Config>, configService);
            auto serviceName = configService->get<std::string>("application.name");
            auto serviceId = discovery::GenerateServiceID(serviceName);
            std::string consulAddr = "http://localhost:8500";
            ushort hostPort = 8083;

            try
            {
                consulAddr = fmt::format("http://{}:{}", 
                    configService->get<std::string>("application.cloud.consul.host"),
                    configService->get<ushort>("application.cloud.consul.port"));
                hostPort = configService->get<ushort>("grpc.server.port");
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }

            spdlog::info(fmt::format("Consul address: {}", consulAddr));

            auto r = discovery::ConsulRegistry::Create(consulAddr);
            r->Register(
                serviceId,
                serviceName,
                std::to_string(hostPort));
            return r;
        }());

        registryService_ = registry;
    }

    void Application::CreateController()
    {
        CREATE_SHARED_SERVICE(std::shared_ptr<controller::Controller>, controller)([]{
            GET_SERVICE(std::shared_ptr<discovery::Registry>, registry);
            auto metadataGateway = std::make_shared<gateway::metadata::grpc::MetadataGateway>(registry);
            auto ratingGateway = std::make_shared<gateway::rating::grpc::RatingGateway>(registry);

            return std::make_shared<controller::Controller>(ratingGateway, metadataGateway);
        }());

        controller_ = controller;
    }
}