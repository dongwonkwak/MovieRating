#include "spdlog/spdlog.h"
#include "movie/gateway/metadata/http/metadata.h"
#include "discovery/consul.h"
using namespace web::http;

namespace movie::gateway::metadata::http
{
    MetadataGateway::MetadataGateway(const std::shared_ptr<discovery::Registry>& registry) noexcept
       : registry_(registry)
    {

    }

    MetadataGateway::~MetadataGateway() noexcept
    {

    }

    auto MetadataGateway::Get(const std::string& id) 
        -> common::expected<::metadata::model::Metadata>
    {
        using namespace common;
        srand(time(NULL));

        auto client = registry_->ServiceAddress("metadata")
            .and_then([=](auto addrs) -> expected<std::string> {
                if (addrs.empty()) { return unexpected{"can't find metadata service"}; }
                return addrs[rand()%addrs.size()];
            })
            .map([=](auto addr){
                return http_client(web::http::uri_builder(addr).append(U("/metadata")).to_uri());
            })
            .map_error([](auto e) { spdlog::error("{}", e); })
            .value();

        utility::ostringstream_t buf;
        buf << U("?id=") << id;
        return client.request(methods::GET, buf.str())
                .then([](http_response resp) { return resp.extract_json(); })
                .then([](web::json::value value) {
                    spdlog::info("response: {}", value.serialize().c_str());
                    return value;
                })
                .then([](web::json::value value) {
                    return ::metadata::model::Metadata::FromJSON(value);
                })
                .get();
    }
}