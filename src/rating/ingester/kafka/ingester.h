#pragma once

#include "rating/model/rating.h"

#include <cppkafka/cppkafka.h>

#include <memory>
#include <string>
#include <vector>

namespace config
{
    class Config;
}

namespace rating::ingester::kafka
{
    class Ingester
    {
    public:
        /**
         * @brief 카프카 객체 생성. 해당 topic에 subscription을 한다.
         * 
         * @param brokers kafka brokers
         * @param groupId kafka group ID
         * @param topic kafka topic
         * @return std::shared_ptr<Ingester> 
         */
        static std::shared_ptr<Ingester> NewIngester(
            const std::string& brokers,
            const std::string& groupId,
            const std::string& topic);
        Ingester(const std::shared_ptr<config::Config>& config);
        /**
         * @brief subcription한 topic에서 데이터가 있는지 확인한다.
         * 
         * @param events 데이터가 있으면 events에 해당 데이터가 insert된다.
         */
        void Poll(RatingEventSet& events);
    private:
        Ingester(std::unique_ptr<cppkafka::Consumer> consumer) noexcept;
    private:
        std::unique_ptr<cppkafka::Consumer> consumer_;
    };
}