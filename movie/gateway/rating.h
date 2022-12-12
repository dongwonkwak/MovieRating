#pragma once

#include "movie/controller/controller.h"
#include "discovery/registry.h"

#include <cpprest/http_client.h>
#include <cpprest/uri.h>
using namespace web;
using namespace web::http::client;

namespace movie
{
    class RatingGateway : public IRatingGateway
    {
    public:
        RatingGateway(std::shared_ptr<discovery::Registry> registry) noexcept;
        common::expected<double> GetAggretatedRating(
                        const std::string& recordID, 
                        const std::string& recordType) override;
        void PutRating(
                    const std::string& recordID, 
                    const std::string& recordType, 
                    const rating::Rating& rating) override;
    private:
        std::shared_ptr<discovery::Registry> registry_;
    };
}