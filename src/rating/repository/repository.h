#pragma once

#include "rating/model/rating.h"

#include "common/expected.h"

#include <vector>
#include <string>

namespace rating::repository
{
    class IRepository
    {
    public:
        using RecordID = std::string;
        using RecordType = std::string;

        virtual ~IRepository() = default;
        virtual auto Get(const RecordID& recordID, const RecordType& recordType)
            -> common::expected<RatingSet> = 0;
        virtual void Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating) = 0;
    };
}