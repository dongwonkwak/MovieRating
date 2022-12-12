#include "repository/memory.h"

namespace rating
{

    common::expected<std::vector<common::expected<Rating>>> Repository::Get(const RecordID& recordID, const RecordType& recordType)
    {
        std::vector<common::expected<Rating>> res;

        auto found1 = repository_.find(recordID);
        if (found1 == repository_.end())
        {
            std::cout << "found1 not\n";
            return common::unexpected{"recordID not found"};
        }
        auto found2 = found1->second.find(recordType);
        if (found2 == found1->second.end())
        {
            std::cout << "found2 not\n";
            return common::unexpected{"recordType not found"};
        }
        
        std::copy(found2->second.begin(), found2->second.end(), std::back_inserter(res));
        std::cout << "??? size: " << res.size() << std::endl;

        return res;
    }

    void Repository::Put(const RecordID& recordId, const RecordType& recordType, const Rating& rating)
    {
        repository_[recordId][recordType].push_back(rating);
    }
}