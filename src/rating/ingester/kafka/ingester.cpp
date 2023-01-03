#include <spdlog/spdlog.h>
#include "rating/ingester/kafka/ingester.h"

#include <cpprest/json.h>
#include "cppcoro/on_scope_exit.hpp"


namespace rating::ingester::kafka
{
    static auto parseRatingEvents(const std::string& line)
        -> common::expected<std::vector<rating::model::RatingEvent>>
    {
        using namespace rating::model;
        std::vector<RatingEvent> res;
        spdlog::debug("Enter parseRatingEvents");

        auto stopOnExit = cppcoro::on_scope_exit([] {
            spdlog::debug("Leave parseRatingEvent");
        });

        try
        {
            spdlog::debug("input: {}", line);
            auto obj = web::json::value::parse(line);
            spdlog::debug("parsed json: {}", obj.serialize().c_str());

            if (!obj.is_array())
            {
                spdlog::debug("not array");
                res.emplace_back(RatingEvent::FromJSON(obj));
                return res;
            }
            spdlog::debug("is array");
            auto objs = obj.as_array();
            for (const auto& o : objs)
            {
                res.emplace_back(RatingEvent::FromJSON(o));
            }
        }
        catch (const std::exception& e)
        {
            return common::unexpected{e.what()};
        }
        return res;
    }

using namespace cppkafka;
////////////////////////
    std::shared_ptr<Ingester> Ingester::NewIngester(
            const std::string& brokers,
            const std::string& groupId,
            const std::string& topic)
    {
        const Configuration config = {
            { "metadata.broker.list", brokers},
            { "group.id", groupId },
            { "enable.auto.commit", false }
        };
        spdlog::info("Create new Ingester");
        spdlog::info("broker.list: {}", brokers);
        spdlog::info("group.id: {}", groupId);
        spdlog::info("topic: {}", topic);

        auto consumer = std::make_unique<Consumer>(config);
        consumer->subscribe({topic});
        auto ingester = std::shared_ptr<Ingester>(new Ingester(std::move(consumer)));
        return ingester;
    }

    void Ingester::Poll(std::vector<rating::model::RatingEvent>& events)
    {
        events.clear();
        Message msg = consumer_->poll(std::chrono::milliseconds(1000));

        if (msg)
        {
            if (msg.get_error())
            {
                if (!msg.is_eof())
                {
                    spdlog::error("[kafka - poll] {}", msg.get_error().to_string());
                }
            }
            else
            {
                parseRatingEvents(msg.get_payload())
                    .map([&](const auto& r) {
                        events.insert(events.begin(), r.begin(), r.end());
                    });
            }
        }
    }

    Ingester::Ingester(std::unique_ptr<cppkafka::Consumer> consumer) noexcept
        : consumer_(std::move(consumer))
    {

    }


}