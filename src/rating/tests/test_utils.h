#pragma once

#include <jthread.hpp>
#include "cppkafka/consumer.h"

#include "rating/model/rating.h"

#include <string>
#include <vector>


auto parseRatings(const std::string& inp) -> std::vector<rating::model::RatingEvent>;

template <typename Service>
class ServiceRunner
{
public:
    ServiceRunner(Service& service, const std::string& addr)
        : service_(service)
    {
        thread_ = std::jthread([&]{
            service_->start(addr);
        });
    }

    void try_join()
    {
        service_->stop();
        if (thread_.joinable())
        {
            thread_.join();
        }
    }
private:
    Service& service_;
    std::jthread thread_;
};