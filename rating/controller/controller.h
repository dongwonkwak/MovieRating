#pragma once

#include "rating/model/rating.h"
#include "common/expected.h"

#include <jthread.hpp>

#include <memory>
#include <vector>

namespace rating::repository
{
    class Repository;
}

namespace rating::ingester::kafka
{
    class Ingester;
}

namespace rating::controller
{
    class Controller
    {
        using RecordID = std::string;
        using RecordType = std::string;
    public:
        explicit Controller(
            std::unique_ptr<rating::repository::Repository> repository,
            std::shared_ptr<rating::ingester::kafka::Ingester> ingester);
        ~Controller();
        auto Get(const RecordID& recordID, const RecordType& recordType)
            -> common::expected<std::vector<common::expected<model::Rating>>>;
        void Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating);
        void StartIngestion();
        void Stop();
    private:
        void doIngestion(const std::stop_token& token);
    private:
        std::unique_ptr<rating::repository::Repository> repository_;
        std::shared_ptr<rating::ingester::kafka::Ingester> ingester_;
        std::jthread ingestThread_;
        bool isStart_ = false;
    };
}