#pragma once

#include "movie/gateway/gateway.h"
#include "discovery/registry.h"


namespace movie::gateway::rating::grpc
{
    class RatingGateway : public IRatingGateway
    {
    public:
        RatingGateway(const std::shared_ptr<discovery::Registry>& registry) noexcept;
        common::expected<double> GetAggretatedRating(
                        const std::string& recordID, 
                        const std::string& recordType) override;
        void PutRating(
                    const std::string& recordID, 
                    const std::string& recordType, 
                    const ::rating::model::Rating& rating) override;
    private:
        std::shared_ptr<discovery::Registry> registry_;
    };
}