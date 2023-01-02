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
        common::expected<model::Metadata> Get(std::string_view id);
        void Put(std::string_view id, const model::Metadata& data);
    private:
        std::map<std::string, model::Metadata> data_;
    };
}