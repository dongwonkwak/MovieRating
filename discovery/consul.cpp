#include "discovery/consul.h"
using namespace ppconsul::agent;

#include <fmt/core.h>
#include <range/v3/all.hpp>

namespace discovery
{
    std::shared_ptr<Registry> ConsulRegistry::Create(const std::string& endpoint, size_t ttl) noexcept
    {
        auto registry = std::shared_ptr<ConsulRegistry>(new ConsulRegistry(endpoint, ttl));
        return registry;
    }

    ConsulRegistry::ConsulRegistry(const std::string& endpoint, size_t ttl)
        : consul_(new ppconsul::Consul(endpoint))
        , health_(new ppconsul::health::Health(*consul_))
        , agent_(new ppconsul::agent::Agent(*consul_))
        , ttl_(ttl)
    {
        if (ttl_ == 0)
        {
            throw std::runtime_error("ttl must over 0");
        }
    }

    ConsulRegistry::~ConsulRegistry()
    {
        Deregister(serviceId_);
    }

    void ConsulRegistry::Register(
        const std::string &serviceId, 
        const std::string &serviceName, 
        const std::string &hostPort)
    {
        serviceId_ = serviceId;

        agent_->registerService(
            kw::name = serviceName,
            kw::port = std::stoi(hostPort),
            kw::id = serviceId,
            kw::check = TtlCheck(std::chrono::seconds(ttl_))
        );
    }
    
    void ConsulRegistry::Deregister(const std::string &serviceId)
    {
        agent_->deregisterService(serviceId);
    }

    void ConsulRegistry::ReportHealthyState(const std::string& serviceId)
    {
        agent_->servicePass(serviceId);
    }

    common::expected<std::vector<std::string>> ConsulRegistry::ServiceAddress(const std::string& serviceName)
    {
        using namespace ranges;
        auto services = agent_->services();
        auto agents = agent_->checks();

        auto rng = agents | ranges::views::values
            | views::filter([&serviceName](auto agent) { 
                return (agent.serviceName == serviceName && agent.status == ppconsul::CheckStatus::Passing);
            })
            | views::transform([&services](auto info) {
                auto s = services.at(info.serviceId);
                return fmt::format("{}:{}", 
                    (s.address.empty() ? "localhost" : s.address), 
                    s.port);
            });
    
    
        auto ret = ranges::to<std::vector<std::string>>(rng);
        if (ret.empty())
        {
            return common::unexpected{fmt::format("can't find [{}] service.", serviceName)};
        }
        return ret;
    }
}