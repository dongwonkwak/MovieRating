#include "movie/gateway/rating.h"

using namespace web::http;

namespace movie
{
    RatingGateway::RatingGateway(std::shared_ptr<discovery::Registry> registry) noexcept
        : registry_(registry)
    {

    }

    common::expected<double> RatingGateway::GetAggretatedRating(
                        const std::string& recordID, 
                        const std::string& recordType)
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

        http_client client(http::uri_builder(addrs.value()[0]).append(U("/rating")).to_uri());
        utility::ostringstream_t buf;
        buf << U("?id=") << recordID << U("&type=") << recordType;

        auto ret = client.request(methods::GET, buf.str())
                .then([](http_response resp) { return resp.extract_string(); })
                .then([](std::string body) {
                    double result = stod(body);
                    std::cout << "!! result: " << result << std::endl;
                    return result;
                });
                
        return ret.get();
    }

    void RatingGateway::PutRating(
                    const std::string& recordID, 
                    const std::string& recordType, 
                    const rating::Rating& rating)
    {
        auto addrs = registry_->ServiceAddress("rating");
        for (const auto& addr : addrs.value())
        {
            std::cout << addr << std::endl;
        }
        if (addrs->empty())
        {
            return;
            //return common::unexpected{"can't find service"};
        }

        http_client client(http::uri_builder(addrs.value()[0]).append(U("/rating")).to_uri());

        utility::ostringstream_t buf;
        buf << U("?id=") << recordID << U("&type=") << recordType
            << U("&userId=") << rating.userId
            << U("&value=") << rating.ratingValue;
        client.request(methods::PUT, buf.str()).get();
        //auto response = client_.request(methods::PUT, buf.str()).get();
    }
}