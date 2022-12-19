#pragma once

#include "model/metadata.h"
#include "common/expected.h"

#include <map>
#include <optional>

namespace metadata::repository
{
    class Repository
    {
    public:
        common::expected<model::Metadata> Get(std::string_view id);
        void Put(std::string_view id, const model::Metadata& data);
    private:
        std::map<std::string, model::Metadata> data_;
    };

    //std::map<std::string, Metadata> data;
}