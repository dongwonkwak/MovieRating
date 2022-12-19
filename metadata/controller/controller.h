#pragma once

#include <memory>
#include "model/metadata.h"
#include "common/expected.h"

namespace metadata::repository
{
    class Repository;
}

namespace metadata::controller
{    
    class Controller
    {
    public:
        explicit Controller(std::unique_ptr<metadata::repository::Repository> repository);
        common::expected<model::Metadata> Get(std::string_view id);
    private:
        std::unique_ptr<metadata::repository::Repository> repository_;
    };
}