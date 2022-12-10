#pragma once

#include "model/rating.h"
#include "common/expected.h"

#include <memory>
#include <vector>

namespace rating
{
    class Repository;

    class Controller
    {
        using RecordID = std::string;
        using RecordType = std::string;
    public:
        explicit Controller(std::unique_ptr<Repository>&& repository);
        common::expected<std::vector<common::expected<Rating>>> Get(const RecordID& recordID, const RecordType& recordType);
        void Put(const RecordID& recordId, const RecordType& recordType, const Rating& rating);
    private:
        std::unique_ptr<Repository> repository_;
    };
}