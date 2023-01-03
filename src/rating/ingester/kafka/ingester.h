#pragma once

#include "rating/model/rating.h"

#include <cppkafka/cppkafka.h>

#include <memory>
#include <string>
#include <vector>

namespace rating::ingester::kafka
{
    class Ingester
    {
    public:
        static std::shared_ptr<Ingester> NewIngester(
            const std::string& brokers,
            const std::string& groupId,
            const std::string& topic);
        void Poll(std::vector<rating::model::RatingEvent>& events);
    private:
        Ingester(std::unique_ptr<cppkafka::Consumer> consumer) noexcept;
    private:
        std::unique_ptr<cppkafka::Consumer> consumer_;
    };
}