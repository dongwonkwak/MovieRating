#include "movie/grpcutil/grpcutil.h"

#include <time.h>
#include <stdio.h>

#include <fmt/core.h>

namespace movie::grpcutil
{
    auto ServiceConnection(const std::string& serviceName, 
                            const std::shared_ptr<discovery::Registry>& registry)
        -> common::expected<std::shared_ptr<grpc::Channel>>
    {
        auto addrs = registry->ServiceAddress(serviceName);
        if (!addrs.has_value())
        {
            return common::unexpected{fmt::format("can't find [{}] service.", serviceName)};
        }
        
        srand(time(NULL));

        return grpc::CreateChannel(
            addrs.value()[rand() % addrs->size()],
            grpc::InsecureChannelCredentials());
    }
}