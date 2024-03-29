#include "spdlog/spdlog.h"
#include "movie/gateway/rating/http/rating.h"

using namespace web::http;

namespace movie::gateway::rating::http
{
    RatingGateway::RatingGateway(const std::shared_ptr<discovery::Registry>& registry) noexcept
        : registry_(registry)
    {

    }

    auto RatingGateway::GetAggretatedRating(
                        const std::string& recordID, 
                        const std::string& recordType)
        -> common::expected<double>
    {
        auto addrs = registry_->ServiceAddress("rating");
        for (const auto& addr : addrs.value())
        {
            std::cout << addr << std::endl;
        }
        if (addrs->empty())
        {
            return common::unexpected{"can't find service"};
        }

        http_client client(web::http::uri_builder(addrs.value()[0]).append(U("/rating")).to_uri());
        utility::ostringstream_t buf;
        buf << U("?id=") << recordID << U("&type=") << recordType;

        auto ret = client.request(methods::GET, buf.str())
                .then([](http_response resp) { return resp.extract_string(); })
                .then([](std::string body) {
                    double result = stod(body);
                    spdlog::info("result: {}", result);
                    return result;
                });
                
        return ret.get();
    }

    void RatingGateway::PutRating(
                    const std::string& recordID, 
                    const std::string& recordType, 
                    const ::rating::model::Rating& rating)
    {
        auto addrs = registry_->ServiceAddress("rating");
        for (const auto& addr : addrs.value())
        {
            spdlog::info("{}", addr);
        }
        if (addrs->empty())
        {
            spdlog::error("can't find service");
            return;
        }

        http_client client(web::http::uri_builder(addrs.value()[0]).append(U("/rating")).to_uri());

        utility::ostringstream_t buf;
        buf << U("?id=") << recordID << U("&type=") << recordType
            << U("&userId=") << rating.userId
            << U("&value=") << rating.ratingValue;
        client.request(methods::PUT, buf.str()).get();
    }
}