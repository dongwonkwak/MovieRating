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
        /**
         * @brief record ID + record type 합성키를 통해 Rating 데이터를 얻는다.
         * 
         * @param recordID 키1
         * @param recordType 키2
         * @return common::expected<RatingSet> 
         */
        auto Get(const RecordID& recordID, const RecordType& recordType)
            -> common::expected<RatingSet> override;
        /**
         * @brief Rating 데이터를 저장한다. 이때 키는 record ID + record type이다.
         * 
         * @param recordId 키1
         * @param recordType 키2
         * @param rating 저장할 rating 데이터
         */
        void Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating) override;
    private:
        std::unique_ptr<::pqxx::connection> conn_;
    }; 
}