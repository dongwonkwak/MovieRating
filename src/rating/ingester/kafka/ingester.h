#pragma once

#include "rating/model/rating.h"

#include "kafka/KafkaConsumer.h"
#include "common/expected.h"

#include <memory>
#include <string>
#include <vector>

namespace rating::ingester::kafka
{
    class Ingester
    {
    public:
        static std::shared_ptr<Ingester> NewIngester(
            const std::string& addr,
            const std::string& groupId,
            const std::string& topic);
        auto Poll()
        -> common::expected<std::vector<rating::model::RatingEvent>>;
    private:
        Ingester(std::unique_ptr<::kafka::clients::consumer::KafkaConsumer> consumer) noexcept;
    private:
        std::unique_ptr<::kafka::clients::consumer::KafkaConsumer> consumer_;
    };
}