#pragma once

#include "discovery/registry.h"
#include <common/expected.h>
#include "movie.grpc.pb.h"

#include <grpcpp/grpcpp.h>

#include <memory>

namespace movie::grpcutil
{

    auto ServiceConnection(const std::string& serviceName, 
                            const std::shared_ptr<discovery::Registry>& registry)
        -> common::expected<std::shared_ptr<grpc::Channel>>;
}