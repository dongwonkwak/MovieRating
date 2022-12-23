#pragma once

#include "rating/repository/IRepository.h"

#include <map>

namespace rating::repository::memory
{
    class Repository : public IRepository
    {
    public:
        auto Get(const RecordID& recordID, const RecordType& recordType)
            -> common::expected<std::vector<common::expected<model::Rating>>> override;
        void Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating) override;
    private:
        std::map<RecordID, std::map<RecordType, std::vector<model::Rating>>> repository_;
    };

    
}