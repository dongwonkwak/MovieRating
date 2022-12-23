#include "rating/repository/memory/memory.h"

namespace rating::repository::memory
{

    auto Repository::Get(const RecordID& recordID, const RecordType& recordType)
        -> common::expected<std::vector<common::expected<model::Rating>>>
    {
        std::vector<common::expected<model::Rating>> res;

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
        
        std::copy(found2->second.begin(), found2->second.end(), std::back_inserter(res));

        return res;
    }

    void Repository::Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating)
    {
        repository_[recordId][recordType].push_back(rating);
    }
}