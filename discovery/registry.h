#pragma once

#include "common/expected.h"

#include <string>
#include <vector>

namespace discovery
{
    struct Registry
    {
        virtual ~Registry() = default;
        virtual void Register(
            const std::string& serviceId, 
            const std::string& serviceName, 
            const std::string& hostPort) = 0;
        virtual void Deregister(const std::string& serviceId) = 0;
        virtual void ReportHealthyState(const std::string& serviceId) = 0;
        virtual common::expected<std::vector<std::string>> ServiceAddress(const std::string& serviceName) = 0;
    };

    std::string GenerateServiceID(const std::string& serviceName);
}