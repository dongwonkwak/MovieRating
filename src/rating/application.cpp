#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "rating/application.h"
#include "rating/repository/postgresql/postgresql.h"
#include "rating/ingester/kafka/ingester.h"
#include "rating/controller/controller.h"
#include "config/config.h"
#include "discovery/consul.h"
#include <map>

static std::map<std::string, spdlog::level::level_enum> logLevelTable {
    {"info", spdlog::level::info},
    {"critical", spdlog::level::critical},
    {"debug", spdlog::level::debug},
    {"trace", spdlog::level::trace},
    {"warn", spdlog::level::warn},
    {"err", spdlog::level::err}
};

static void InitializeLogger(const std::shared_ptr<config::Config>& config)
{
    auto pattern = config->get<std::string>("logging.pattern.console");
    if (!pattern.empty())
    {
        spdlog::set_pattern(pattern);
    }
    auto level = config->get<std::string>("logging.level");
    auto level_iter = logLevelTable.find(level);
    if (level_iter != logLevelTable.end())
    {
        spdlog::set_level(level_iter->second);
    }
    spdlog::info("InitializeLogger");
}
namespace rating
{
    std::shared_ptr<Hypodermic::Container> Application::operator()()
    {
        return container_;
    }

    Application::Application(const boost::program_options::variables_map& vm)
    {
        std::string configFile = vm["config-file"].as<std::string>();
        auto configService = config::Config::Create(configFile);
        InitializeLogger(configService);

        builder_.registerInstance(configService);

        CreateRegistry();   
        CreateController();

        container_ = builder_.build();
    }
    

    void Application::CreateRegistry()
    {
        builder_.registerInstanceFactory([](Hypodermic::ComponentContext& context){
            auto configService = context.resolve<config::Config>();
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
        });   
    }

    void Application::CreateController()
    {
        builder_.registerInstanceFactory([](Hypodermic::ComponentContext& context) {
            auto configService = context.resolve<config::Config>();
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
            return std::make_shared<controller::Controller>(repository, ingester);
        });
    }
}