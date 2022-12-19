#pragma once

#include "model/rating.h"
#include "common/expected.h"

#include <map>
#include <vector>

namespace rating::repository
{
    class Repository
    {
        using RecordID = std::string;
        using RecordType = std::string;
    public:
        auto Get(const RecordID& recordID, const RecordType& recordType)
            -> common::expected<std::vector<common::expected<model::Rating>>>;
        void Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating);
    private:
        std::map<RecordID, std::map<RecordType, std::vector<model::Rating>>> repository_;
    };

    
}