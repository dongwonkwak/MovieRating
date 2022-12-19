#pragma once

#include "movie/model/movie.h"
#include "rating/model/rating.h"

#include <common/expected.h>

#include <string>

namespace movie::gateway
{
    struct IRatingGateway
    {
        virtual ~IRatingGateway() = default;
        virtual auto GetAggretatedRating(
                        const std::string& recordID, 
                        const std::string& recordType)
                -> common::expected<double> = 0;
        virtual void PutRating(
                    const std::string& recordID, 
                    const std::string& recordType, 
                    const rating::model::Rating& rating) = 0;
    };

    struct IMetadataGateway
    {
        virtual ~IMetadataGateway() = default;
        virtual auto Get(const std::string& id) 
            -> common::expected<metadata::model::Metadata> = 0;
    };
}