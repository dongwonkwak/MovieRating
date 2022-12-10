#include "controller.h"
#include "repository/repository.h"

namespace metadata
{
Controller::Controller(std::unique_ptr<Repository>&& repository)
    : repository_{std::move(repository)}
{

}

common::expected<Metadata> Controller::Get(std::string_view id)
{
    return repository_->Get(id);
}


}