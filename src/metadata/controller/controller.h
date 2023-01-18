#pragma once

#include <memory>
#include "metadata/model/metadata.h"
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
    /**
     * @brief Construct a new Controller object (business logic)
     * 
     * @param repository metadata repository
     */
        explicit Controller(const std::shared_ptr<metadata::repository::Repository>& repository);
        common::expected<model::Metadata> Get(std::string_view id);
    private:
        std::shared_ptr<metadata::repository::Repository> repository_;
    };
}