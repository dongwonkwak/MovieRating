#include "movie/gateway/metadata.h"

using namespace web::http;

namespace movie
{
    MetadataGateway::MetadataGateway(utility::string_t url)
        : client_(http::uri_builder(url).append_path(U("/metadata")).to_uri())
    {

    }

    auto MetadataGateway::Get(const std::string& id) 
        -> common::expected<metadata::Metadata>
    {
        utility::ostringstream_t buf;
        buf << U("?id=") << id;
        auto response = client_.request(methods::GET, buf.str()).get();
        ucout << response.to_string() << std::endl;

        if (response.to_string().empty())
        {
            return common::unexpected{"response is empty"};
        }

        return metadata::Metadata::FromJSON(response.extract_json().get());
    }
}