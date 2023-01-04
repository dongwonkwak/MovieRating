#pragma once

#include <jthread.hpp>
#include "cppkafka/consumer.h"

#include "rating/model/rating.h"

#include <string>
#include <vector>


auto parseRatings(const std::string& inp) -> std::vector<rating::model::RatingEvent>;

template <typename ConsumerType>
class ConsumerRunner
{
public:
    ConsumerRunner(ConsumerType& consumer)
        : consumer_(consumer)
    {
        thread_ = std::jthread([&](const std::stop_token& token){
            RatingEventSet ret;

            while (true)
            {
                consumer_->Poll(ret);
                if (!ret.empty())
                {
                    std::copy(std::begin(ret), std::end(ret),
                            std::inserter(events_, std::end(events_)));
                }

                if (token.stop_requested())
                {
                    break;
                }
            }
        });
    }
    const RatingEventSet& get_events() const
    {
        return events_;
    }

    void try_join()
    {
        thread_.request_stop();
        if (thread_.joinable())
        {
            thread_.join();
        }
    }
private:
    ConsumerType& consumer_;
    RatingEventSet events_;
    std::jthread thread_;
};