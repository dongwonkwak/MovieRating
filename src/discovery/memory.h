#pragma once

#include "discovery/registry.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <mutex>
#include <chrono>

namespace discovery
{
    // in-memory registry
    class MemoryRegistry : public Registry
    {
        using time_t = std::chrono::time_point<std::chrono::system_clock>;
        struct ServiceInstance
        {
            std::string service_id;
            std::string service_name;
            std::string host_port;
            time_t expiration;
            
            ServiceInstance(
                const std::string& serviceId, 
                const std::string& serviceName, 
                const std::string& hostPort,
                int64_t delay)
                : service_id(serviceId)
                , service_name(serviceName)
                , host_port(hostPort)
            {
                expiration = std::chrono::system_clock::now() + std::chrono::seconds(delay);
            }

            bool operator<(const ServiceInstance& other) const
            {
                return expiration < other.expiration;
            }
        };
    public:
        MemoryRegistry(int64_t ttl = 5);
        ~MemoryRegistry();

        void Register(const std::string& serviceId, const std::string& serviceName, const std::string& hostPort) override;
        void Deregister(const std::string& serviceId) override;
        void ReportHealthyState(const std::string& serviceId) override;
        common::expected<std::vector<std::string>> ServiceAddress(const std::string& serviceName) override;
    private:
        struct TimestampTag {};
        struct ServiceNameTag {};
        struct ServiceIdTag {};
        using Set = boost::multi_index::multi_index_container<
            ServiceInstance,
            boost::multi_index::indexed_by<
                boost::multi_index::ordered_non_unique<
                    boost::multi_index::tag<ServiceNameTag>,
                    boost::multi_index::member<ServiceInstance, std::string, &ServiceInstance::service_name>
                >,
                boost::multi_index::ordered_unique<
                    boost::multi_index::tag<ServiceIdTag>,
                    boost::multi_index::member<ServiceInstance, std::string, &ServiceInstance::service_id>
                >,
                boost::multi_index::ordered_non_unique<
                    boost::multi_index::tag<TimestampTag>,
                    boost::multi_index::member<ServiceInstance, time_t, &ServiceInstance::expiration>
                >
            >
        >;
        std::mutex mutex_;
        int64_t ttl_ = 5;
        Set set_;
    };
}
