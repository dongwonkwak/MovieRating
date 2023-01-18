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
        // 동일할 서비스가 여러개인 경우 랜덤함수를 이용해 무작위로 결정한다.
        return registry->ServiceAddress(serviceName)
            .map([=](auto addrs) {
                return grpc::CreateChannel(
                    addrs[rand() % addrs.size()],
                    grpc::InsecureChannelCredentials());
            });
    }
}