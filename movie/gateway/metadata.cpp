#include "movie/gateway/metadata.h"
#include "discovery/consul.h"
using namespace web::http;

namespace movie
{
    MetadataGateway::MetadataGateway(std::shared_ptr<discovery::Registry> registry) noexcept
       : registry_(registry)
    {

    }

    MetadataGateway::MetadataGateway(MetadataGateway&& other)
        : registry_(std::move(other.registry_))
    {

    }

    MetadataGateway& MetadataGateway::operator=(MetadataGateway&& other)
    {
        registry_ = std::move(other.registry_);
        return *this;
    }

    MetadataGateway::~MetadataGateway() noexcept
    {

    }

    auto MetadataGateway::Get(const std::string& id) 
        -> common::expected<metadata::Metadata>
    {
        auto addrs = registry_->ServiceAddress("metadata");
        for (const auto& addr : addrs.value())
        {
            std::cout << addr << std::endl;
        }
        if (addrs->empty())
        {
            return common::unexpected{"can't find service"};
        }
        http_client client(http::uri_builder(addrs.value()[0]).append(U("/metadata")).to_uri());
        utility::ostringstream_t buf;
        buf << U("?id=") << id;
        auto response = client.request(methods::GET, buf.str()).get();
        ucout << response.to_string() << std::endl;

        if (response.to_string().empty())
        {
            return common::unexpected{"response is empty"};
        }

        return metadata::Metadata::FromJSON(response.extract_json().get());

    }
}