#pragma once

#include "discovery/registry.h"
#include <ppconsul/agent.h>
#include <ppconsul/health.h>
#include <common/expected.h>

#include <memory>

namespace discovery
{
    class ConsulRegistry : public Registry
    {
    public:
        static std::shared_ptr<Registry> Create(
            const std::string& endpoint = "http://127.0.0.1:8500", 
            size_t ttl = 5) noexcept;
        
        ~ConsulRegistry();

        void Register(const std::string& serviceId, const std::string& serviceName, const std::string& hostPort) override;
        void Deregister(const std::string& serviceId) override;
        void ReportHealthyState(const std::string& serviceId) override;
        common::expected<std::vector<std::string>> ServiceAddress(const std::string& serviceName) override;
    private:
        ConsulRegistry(const std::string& endpoint, size_t ttl);
    private:
        std::unique_ptr<ppconsul::Consul> consul_;
        std::unique_ptr<ppconsul::agent::Agent> agent_;
        std::unique_ptr<ppconsul::health::Health> health_;
        size_t ttl_;
        std::string serviceId_;
    };
}