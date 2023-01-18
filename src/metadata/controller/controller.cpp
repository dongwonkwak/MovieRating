#include "controller.h"
#include "metadata/repository/repository.h"

namespace metadata::controller
{
Controller::Controller(const std::shared_ptr<metadata::repository::IRepository>& repository)
    : repository_{repository}
{

}

common::expected<model::Metadata> Controller::Get(std::string_view id)
{
    return repository_->Get(id);
}


}