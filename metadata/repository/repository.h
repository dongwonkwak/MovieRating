#pragma once

#include "model/metadata.h"
#include "common/expected.h"

#include <map>
#include <optional>

namespace metadata
{
    class Repository
    {
    public:
        common::expected<Metadata> Get(std::string_view id);
        void Put(std::string_view id, const Metadata& data);
    private:
        std::map<std::string, Metadata> data_;
    };

    //std::map<std::string, Metadata> data;
}