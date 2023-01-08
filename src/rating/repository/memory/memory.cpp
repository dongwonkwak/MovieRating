#include "rating/repository/memory/memory.h"

namespace rating::repository::memory
{

    auto Repository::Get(const RecordID& recordID, const RecordType& recordType)
        -> common::expected<RatingSet>
    {
        RatingSet res;

        auto found1 = repository_.find(recordID);
        if (found1 == repository_.end())
        {
            return common::unexpected{"recordID not found"};
        }
        auto found2 = found1->second.find(recordType);
        if (found2 == found1->second.end())
        {
            return common::unexpected{"recordType not found"};
        }
        
        std::copy(std::begin(found2->second), std::end(found2->second), 
            std::inserter(res, std::end(res)));

        return res;
    }

    void Repository::Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating)
    {
        repository_[recordId][recordType].push_back(rating);
    }
}