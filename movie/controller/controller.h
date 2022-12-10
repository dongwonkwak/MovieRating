#pragma once

#include "common/expected.h"
#include "rating/model/rating.h"
#include "metadata/model/metadata.h"
#include "movie/model/movie.h"

#include <string>
#include <memory>

namespace movie
{
    struct IRatingGateway
    {
        virtual ~IRatingGateway() = default;
        virtual common::expected<double> GetAggretatedRating(
                        const std::string& recordID, 
                        const std::string& recordType) = 0;
        virtual void PutRating(
                    const std::string& recordID, 
                    const std::string& recordType, 
                    const rating::Rating& rating) = 0;
    };

    struct IMetadataGateway
    {
        virtual ~IMetadataGateway() = default;
        virtual auto Get(const std::string& id) 
            -> common::expected<metadata::Metadata> = 0;
    };

    class Controller
    {
    public:
        explicit Controller(
            std::unique_ptr<IRatingGateway>&& ratingGateway, 
            std::unique_ptr<IMetadataGateway>&& metadataGateway);
        auto Get(const std::string& id) -> common::expected<MovieDetails>;
    private:
        std::unique_ptr<IRatingGateway> ratingGateway_;
        std::unique_ptr<IMetadataGateway> metadataGateway_;
    };
}