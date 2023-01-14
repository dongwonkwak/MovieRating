#include "movie/grpcutil/grpcutil.h"

#include <fmt/core.h>

#include <time.h>
#include <stdio.h>

namespace movie::grpcutil
{
    auto ServiceConnection(const std::string& serviceName, 
                            const std::shared_ptr<discovery::Registry>& registry)
        -> common::expected<std::shared_ptr<grpc::Channel>>
    {
        srand(time(NULL));

        return registry->ServiceAddress(serviceName)
            .map([=](auto addrs) {
                return grpc::CreateChannel(
                    addrs[rand() % addrs.size()],
                    grpc::InsecureChannelCredentials());
            });
    }
}