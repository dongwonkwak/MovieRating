#include <fmt/core.h>

#include "movie/AppComponent.h"
#include "config/ServiceProvider.h"
#include "discovery/consul.h"
#include "config/config.h"

#include <spdlog/spdlog.h>

#include <map>

namespace movie
{
    static std::map<std::string, spdlog::level::level_enum> logLevelTable {
        {"info", spdlog::level::info},
        {"critical", spdlog::level::critical},
        {"debug", spdlog::level::debug},
        {"trace", spdlog::level::trace}
    };

    static void initLogger(const std::shared_ptr<config::Config>& config)
    {
        spdlog::set_pattern(config->get<std::string>("logging.pattern.console"));
        auto level = config->get<std::string>("logging.level");
        spdlog::set_level(logLevelTable[level]);
        spdlog::info("initLogger");
    }

    AppComponent::AppComponent(const std::string& filename)
        : configFile_{filename}
    {

    }
    
    /*AppComponent::AppComponent(const boost::program_options::variables_map& vm)
        : vm_{vm}
    {
        CREATE_SERVICE(std::shared_ptr<config::Config>, configService)([this]{
            return config::Config::Create();
        }());
        
        std::string filename = vm_["config-file"].as<std::string>();
        spdlog::info("{}", filename);

        try
        {
            configService.getObject()->Load(filename);
        }
        catch(std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
      
        initLogger(configService.getObject());
        CreateConsulService();
    }

    void AppComponent::CreateConfigService()
    {

    }

    void AppComponent::CreateConsulService()
    {
        GET_SERVICE(std::shared_ptr<config::Config>, config);
        auto serviceName = config->get<std::string>("application.name");
        auto serviceId = discovery::GenerateServiceID(serviceName);

        std::string consulAddr = "http://localhost:8500";
        ushort hostPort = 8083;

        try
        {
            consulAddr = fmt::format("http://{}:{}", 
                config->get<std::string>("application.cloud.consul.host"),
                config->get<ushort>("application.cloud.consul.port"));
            hostPort = config->get<ushort>("grpc.server.port");
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        spdlog::info(fmt::format("Consul address: {}", consulAddr));

        CREATE_SERVICE(std::shared_ptr<discovery::Registry>, registry)([=]{
            auto r = discovery::ConsulRegistry::Create(consulAddr);
            r->Register(
                serviceId,
                serviceName,
                std::to_string(hostPort));
            return r;
        }());
    }*/
}