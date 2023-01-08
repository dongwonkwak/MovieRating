#pragma once

#include "rating/repository/IRepository.h"
#include <pqxx/pqxx>

#include <memory>

namespace config
{
    class Config;
}

namespace rating::repository::postresql
{
    class Repository : public IRepository
    {
    public:
        Repository(std::string_view host, unsigned short port, std::string_view dbname, std::string_view user, std::string_view password);
        Repository(const std::string& connStr);
        Repository(const std::shared_ptr<config::Config>& config);
        
        auto Get(const RecordID& recordID, const RecordType& recordType)
            -> common::expected<RatingSet> override;
        void Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating) override;
    private:
        std::unique_ptr<::pqxx::connection> conn_;
    }; 
}