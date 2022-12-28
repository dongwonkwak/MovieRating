#pragma once

#include "movie/gateway/gateway.h"
#include "discovery/registry.h"

#include <memory>

namespace movie::gateway::metadata::grpc
{
    class MetadataGateway : public IMetadataGateway
    {
    public:
        MetadataGateway(const std::shared_ptr<discovery::Registry>& registry) noexcept;
        ~MetadataGateway() noexcept;
        auto Get(const std::string& id) 
            -> common::expected<::metadata::model::Metadata> override;        
    private:
        std::shared_ptr<discovery::Registry> registry_;
    };
}