#include "discovery/memory.h"

namespace discovery
{
    MemoryRegistry::MemoryRegistry()
    {

    }

    MemoryRegistry::~MemoryRegistry()
    {

    }

    void MemoryRegistry::Register(const std::string& serviceId, const std::string& serviceName, const std::string& hostPort)
    {
        std::unique_lock lock(mutex_);

    }

    void MemoryRegistry::Deregister(const std::string& serviceId)
    {
        std::unique_lock lock(mutex_);
    }

    void MemoryRegistry::ReportHealthyState(const std::string& serviceId)
    {

    }

    common::expected<std::vector<std::string>> MemoryRegistry::ServiceAddress(const std::string& serviceName)
    {
        return std::vector<std::string>{};
    }
}