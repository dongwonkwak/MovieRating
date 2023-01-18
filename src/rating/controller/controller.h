#pragma once

#include "rating/model/rating.h"
#include "rating/repository/repository.h"
#include "common/expected.h"

#include <jthread.hpp>

#include <memory>
#include <vector>


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
        Controller(
            std::shared_ptr<rating::repository::IRepository> repository,
            std::shared_ptr<rating::ingester::kafka::Ingester> ingester);
        ~Controller();
        auto Get(const RecordID& recordID, const RecordType& recordType)
            -> common::expected<RatingSet>;
        void Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating);
        void StartIngestion();
        void Stop();
    private:
        void doIngestion(const std::stop_token& token);
    private:
        std::shared_ptr<rating::repository::IRepository> repository_;
        std::shared_ptr<rating::ingester::kafka::Ingester> ingester_;
        std::jthread ingestThread_;
        bool isStart_ = false;
    };
}