#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "rating/application.h"
#include "rating/repository/postgresql/postgresql.h"
#include "rating/ingester/kafka/ingester.h"

#include <map>

static std::map<std::string, spdlog::level::level_enum> logLevelTable {
        {"info", spdlog::level::info},
        {"critical", spdlog::level::critical},
        {"debug", spdlog::level::debug},
        {"trace", spdlog::level::trace},
        {"warn", spdlog::level::warn},
        {"err", spdlog::level::err}
    };

namespace rating
{
    Application::Application(const boost::program_options::variables_map& vm)
    {
        std::string configFile = vm["config-file"].as<std::string>();

        CREATE_SHARED_SERVICE(std::shared_ptr<config::Config>, config)([=]{
            return config::Config::Create(configFile);
        }());

        configService_ = config;

        InitializeLogger();
        CreateRegistry();   
        CreateController();
    }
    
    void Application::InitializeLogger()
    {
        GET_SERVICE(std::shared_ptr<config::Config>, configService);

        auto pattern = configService->get<std::string>("logging.pattern.console");
        if (!pattern.empty())
        {
            spdlog::set_pattern(pattern);
        }
        auto level = configService->get<std::string>("logging.level");
        auto level_iter = logLevelTable.find(level);
        if (level_iter != logLevelTable.end())
        {
            spdlog::set_level(level_iter->second);
        }
        spdlog::info("InitializeLogger");
    }

    void Application::CreateRegistry()
    {
        CREATE_SHARED_SERVICE(std::shared_ptr<discovery::Registry>, registry)([]{
            GET_SERVICE(std::shared_ptr<config::Config>, configService);
            auto serviceName = configService->get<std::string>("application.name");
            auto serviceId = discovery::GenerateServiceID(serviceName);
            std::string consulAddr = "http://localhost:8500";
            ushort hostPort = 8082;

            try
            {
                consulAddr = fmt::format("http://{}:{}", 
                    configService->get<std::string>("application.cloud.consul.host"),
                    configService->get<ushort>("application.cloud.consul.port"));
                hostPort = configService->get<ushort>("grpc.server.port");
            }
            catch(const std::exception& e)
            {
                spdlog::error(e.what());
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
            GET_SERVICE(std::shared_ptr<config::Config>, configService);
            auto connectString = 
                fmt::format("postgresql://{}:{}@{}",
                            configService->get<std::string>("application.datasource.user"),
                            configService->get<std::string>("application.datasource.password"),
                            configService->get<std::string>("application.datasource.url"));
            auto repository = std::make_shared<repository::postresql::Repository>(connectString);
            auto kafkaHost = configService->get<std::string>("application.kafka.consumer.bootstrap-servers");
            auto groupId = configService->get<std::string>("application.kafka.consumer.group-id");
            const std::string topic = "ratings";

            auto ingester = ingester::kafka::Ingester::NewIngester(kafkaHost, groupId, topic);

            return std::make_shared<controller::Controller>(std::move(repository), ingester);
        }());

        controller_ = controller;
    }
}