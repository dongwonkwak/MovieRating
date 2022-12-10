#pragma once

#include "movie/controller/controller.h"

#include <cpprest/http_client.h>
#include <cpprest/uri.h>
using namespace web;
using namespace web::http::client;

namespace movie
{
    class MetadataGateway : public IMetadataGateway
    {
    public:
        MetadataGateway(utility::string_t url);
        auto Get(const std::string& id) 
            -> common::expected<metadata::Metadata> override;
    private:
        http_client client_;
    };
}