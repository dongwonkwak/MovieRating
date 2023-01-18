#pragma once


#include "metadata/model/metadata.h"
#include "metadata/repository/repository.h"
#include "common/expected.h"

#include <memory>


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
        explicit Controller(const std::shared_ptr<metadata::repository::IRepository>& repository);
        common::expected<model::Metadata> Get(std::string_view id);
    private:
        std::shared_ptr<metadata::repository::IRepository> repository_;
    };
}