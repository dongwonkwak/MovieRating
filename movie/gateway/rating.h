#pragma once

#include "movie/controller/controller.h"

#include <cpprest/http_client.h>
#include <cpprest/uri.h>
using namespace web;
using namespace web::http::client;

namespace movie
{
    class RatingGateway : public IRatingGateway
    {
    public:
        RatingGateway(utility::string_t url);
        common::expected<double> GetAggretatedRating(
                        const std::string& recordID, 
                        const std::string& recordType) override;
        void PutRating(
                    const std::string& recordID, 
                    const std::string& recordType, 
                    const rating::Rating& rating) override;
    private:
        http_client client_;
    };
}