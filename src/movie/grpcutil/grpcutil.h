#pragma once

#include "discovery/registry.h"
#include <common/expected.h>
#include "movie.grpc.pb.h"

#include <grpcpp/grpcpp.h>

#include <memory>

namespace movie::grpcutil
{
    /**
     * @brief 서비스 이름에 해당하는 grpc 채널을 리턴한다.
     * 
     * @param serviceName 
     * @param registry 
     * @return common::expected<std::shared_ptr<grpc::Channel>> 
     */
    auto ServiceConnection(const std::string& serviceName, 
                            const std::shared_ptr<discovery::Registry>& registry)
        -> common::expected<std::shared_ptr<grpc::Channel>>;
}