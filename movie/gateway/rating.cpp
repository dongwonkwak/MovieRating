#include "movie/gateway/rating.h"

using namespace web::http;

namespace movie
{
    RatingGateway::RatingGateway(utility::string_t url)
        : client_(http::uri_builder(url).append_path(U("/rating")).to_uri())
    {

    }

    common::expected<double> RatingGateway::GetAggretatedRating(
                        const std::string& recordID, 
                        const std::string& recordType)
    {
        utility::ostringstream_t buf;
        buf << U("?id=") << recordID << U("&type=") << recordType;
        auto response = client_.request(methods::GET, buf.str()).get();
        ucout << response.to_string() << std::endl;

        if (response.to_string().empty())
        {
            return common::unexpected{"response is empty"};
        }

        double result = stod(response.to_string());
        return result;
    }

    void RatingGateway::PutRating(
                    const std::string& recordID, 
                    const std::string& recordType, 
                    const rating::Rating& rating)
    {
        utility::ostringstream_t buf;
        buf << U("?id=") << recordID << U("&type=") << recordType
            << U("&userId=") << rating.userId
            << U("&value=") << rating.ratingValue;
        
        auto response = client_.request(methods::PUT, buf.str()).get();
    }
}