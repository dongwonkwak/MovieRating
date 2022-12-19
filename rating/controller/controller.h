#pragma once

#include "model/rating.h"
#include "common/expected.h"

#include <memory>
#include <vector>

namespace rating::repository
{
    class Repository;
}

namespace rating::controller
{
    class Controller
    {
        using RecordID = std::string;
        using RecordType = std::string;
    public:
        explicit Controller(std::unique_ptr<rating::repository::Repository> repository);
        auto Get(const RecordID& recordID, const RecordType& recordType)
            -> common::expected<std::vector<common::expected<model::Rating>>>;
        void Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating);
    private:
        std::unique_ptr<rating::repository::Repository> repository_;
    };
}