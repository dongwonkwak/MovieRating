#pragma once

#include <memory>
#include "model/metadata.h"
#include "common/expected.h"

namespace metadata
{
    class Repository;

    class Controller
    {
    public:
        explicit Controller(std::unique_ptr<Repository>&& repository);
        common::expected<Metadata> Get(std::string_view id);
    private:
        std::unique_ptr<Repository> repository_;
    };
}