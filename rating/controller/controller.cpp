#include "controller/controller.h"
#include "repository/memory.h"

namespace rating::controller
{
    Controller::Controller(std::unique_ptr<rating::repository::Repository> repository)
        : repository_{std::move(repository)}
    {

    }

    auto Controller::Get(const RecordID& recordID, const RecordType& recordType)
        -> common::expected<std::vector<common::expected<model::Rating>>> 
    {
        return repository_->Get(recordID, recordType);
    }

    void Controller::Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating)
    {
        repository_->Put(recordId, recordType, rating);
    }
}