#pragma once

#include "discovery/registry.h"
#include <map>
#include <mutex>

namespace discovery
{
    class MemoryRegistry : public Registry
    {
    public:
        MemoryRegistry();
        ~MemoryRegistry();

        void Register(const std::string& serviceId, const std::string& serviceName, const std::string& hostPort) override;
        void Deregister(const std::string& serviceId) override;
        void ReportHealthyState(const std::string& serviceId) override;
        common::expected<std::vector<std::string>> ServiceAddress(const std::string& serviceName) override;
    private:
        std::mutex mutex_;
    };
}