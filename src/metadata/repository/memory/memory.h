#pragma once

#include "metadata/repository/repository.h"
#include <map>

namespace metadata::repository::memory
{
    class Repository : public IRepository
    {
    public:
        /**
         * @brief id에 해당하는 movie metadata값을 얻는다.
         * 
         * @param id 조회할 id
         * @return common::expected<model::Metadata> 
         */
        auto Get(std::string_view id) -> common::expected<model::Metadata> override;

        /**
         * @brief movie metadata값을 저장한다.
         * 
         * @param id 저장할 movie id(key)
         * @param data 저장할 movie metadata(value)
         */
        void Put(std::string_view id, const model::Metadata& data) override;
    private:
        std::map<std::string, model::Metadata> data_;
    };
}