#pragma once

#include "metadata/model/metadata.h"
#include "common/expected.h"

#include <map>
#include <optional>

namespace metadata::repository
{
    class Repository
    {
    public:
        /**
         * @brief id에 해당하는 movie metadata값을 얻는다.
         * 
         * @param id 조회할 id
         * @return common::expected<model::Metadata> 
         */
        common::expected<model::Metadata> Get(std::string_view id);

        /**
         * @brief movie metadata값을 저장한다.
         * 
         * @param id 저장할 movie id(key)
         * @param data 저장할 movie metadata(value)
         */
        void Put(std::string_view id, const model::Metadata& data);
    private:
        std::map<std::string, model::Metadata> data_;
    };
}