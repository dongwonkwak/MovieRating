#include "controller/controller.h"
#include "repository/memory.h"

namespace rating
{
    Controller::Controller(std::unique_ptr<Repository>&& repository)
        : repository_{std::move(repository)}
    {

    }

    common::expected<std::vector<common::expected<Rating>>> Controller::Get(const RecordID& recordID, const RecordType& recordType)
    {
        return repository_->Get(recordID, recordType);
    }

    void Controller::Put(const RecordID& recordId, const RecordType& recordType, const Rating& rating)
    {
        repository_->Put(recordId, recordType, rating);
    }
}