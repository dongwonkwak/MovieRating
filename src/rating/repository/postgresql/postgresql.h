#pragma once

#include "rating/repository/IRepository.h"
#include <pqxx/pqxx>

#include <memory>


namespace rating::repository::postresql
{
    class Repository : public IRepository
    {
    public:
        explicit Repository(const std::string& connStr);
        auto Get(const RecordID& recordID, const RecordType& recordType)
            -> common::expected<std::vector<common::expected<model::Rating>>> override;
        void Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating) override;
    private:
        std::unique_ptr<::pqxx::connection> conn_;
    }; 
}