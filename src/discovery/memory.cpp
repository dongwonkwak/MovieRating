#include "discovery/memory.h"

namespace discovery
{
    MemoryRegistry::MemoryRegistry(int64_t ttl)
        : ttl_(ttl)
    {

    }

    MemoryRegistry::~MemoryRegistry()
    {

    }

    void MemoryRegistry::Register(const std::string& serviceId, const std::string& serviceName, const std::string& hostPort)
    {
        std::unique_lock lock(mutex_);
        set_.insert({serviceId, serviceName, hostPort, ttl_});
    }

    void MemoryRegistry::Deregister(const std::string& serviceId)
    {
        std::unique_lock lock(mutex_);
        set_.get<ServiceIdTag>().erase(serviceId);
    }

    void MemoryRegistry::ReportHealthyState(const std::string& serviceId)
    {
        std::unique_lock lock(mutex_);
        auto& byExpiration = set_.get<TimestampTag>();
        auto now = std::chrono::system_clock::now();
        const auto end = byExpiration.upper_bound(now);
        std::vector<ServiceInstance> expired;
        std::move(byExpiration.begin(), end, std::back_inserter(expired));
        byExpiration.erase(byExpiration.begin(), end);

        auto& serviceIds = set_.get<ServiceIdTag>();
        auto iter = serviceIds.find(serviceId);
        if (iter == serviceIds.end())
        {
            return;
        }

        // update timestamp
        serviceIds.modify(iter, [=](auto& s){
            s.expiration = now + std::chrono::seconds(ttl_);
        });
    }

    common::expected<std::vector<std::string>> MemoryRegistry::ServiceAddress(const std::string& serviceName)
    {
        std::unique_lock lock(mutex_);
        std::vector<std::string> res;
        auto range = set_.get<ServiceNameTag>().equal_range(serviceName);
        for (auto iter = range.first; iter != range.second; ++iter)
        {
            res.push_back(iter->host_port);
        }

        return res;
    }
}