#pragma once

#include "movie/controller/controller.h"
#include "discovery/registry.h"

#include <cpprest/http_client.h>
#include <cpprest/uri.h>
using namespace web;
using namespace web::http::client;

#include <memory>

namespace movie
{
    class MetadataGateway : public IMetadataGateway
    {
    public:
        MetadataGateway(std::shared_ptr<discovery::Registry> registry) noexcept;
        ~MetadataGateway() noexcept;
        auto Get(const std::string& id) 
            -> common::expected<metadata::Metadata> override;
        
        MetadataGateway(MetadataGateway&& other);
        MetadataGateway& operator=(MetadataGateway&& other);
    private:
        std::shared_ptr<discovery::Registry> registry_;
    };
}