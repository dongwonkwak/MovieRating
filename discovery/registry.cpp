#include "discovery/registry.h"

#include <fmt/core.h>

#include <cstdlib>

namespace discovery
{
    std::string GenerateServiceID(const std::string& serviceName)
    {
        srand((unsigned)time(NULL));

        return fmt::format("{}-{}", serviceName, rand());
    }
}