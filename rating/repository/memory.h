#pragma once

#include "model/rating.h"
#include "common/expected.h"

#include <map>
#include <vector>

namespace rating
{
    class Repository
    {
        using RecordID = std::string;
        using RecordType = std::string;
    public:
        common::expected<std::vector<common::expected<Rating>>> Get(const RecordID& recordID, const RecordType& recordType);
        void Put(const RecordID& recordId, const RecordType& recordType, const Rating& rating);
    private:
        std::map<RecordID, std::map<RecordType, std::vector<Rating>>> repository_;
    };

    
}