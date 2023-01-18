#pragma once

#include "metadata/model/metadata.h"
#include "common/expected.h"

#include <string>


namespace metadata::repository
{
    struct IRepository
    {
        virtual ~IRepository() = default;
        virtual auto Get(std::string_view id) -> common::expected<model::Metadata> = 0;
        virtual void Put(std::string_view id, const model::Metadata& data) = 0;
    };
}