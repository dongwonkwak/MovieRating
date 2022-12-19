#include "controller.h"
#include "repository/repository.h"

namespace metadata::controller
{
Controller::Controller(std::unique_ptr<metadata::repository::Repository> repository)
    : repository_{std::move(repository)}
{

}

common::expected<model::Metadata> Controller::Get(std::string_view id)
{
    return repository_->Get(id);
}


}