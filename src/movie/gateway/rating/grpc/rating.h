#pragma once

#include "movie/gateway/gateway.h"
#include "discovery/registry.h"


namespace movie::gateway::rating::grpc
{
    class RatingGateway : public IRatingGateway
    {
    public:
        /**
         * @brief Construct a new Rating Gateway object
         * 
         * @param registry 서비스 레지스트리
         */
        RatingGateway(const std::shared_ptr<discovery::Registry>& registry) noexcept;
        /**
         * @brief Get the Aggretated Rating object
         * key는 record ID + record type이다.
         * 
         * @param recordID 조회할 record ID
         * @param recordType 조회할 record Type
         * @return common::expected<double> 
         */
        auto GetAggretatedRating(
                const std::string& recordID, 
                const std::string& recordType) -> common::expected<double> override;
        /**
         * @brief Rating 정보를 저장한다. 키는 record ID, record type
         * 
         * @param recordID key1
         * @param recordType key2
         * @param rating 저장할 rating 정보
         */
        void PutRating(
                const std::string& recordID, 
                const std::string& recordType, 
                const ::rating::model::Rating& rating) override;
    private:
        std::shared_ptr<discovery::Registry> registry_;
    };
}