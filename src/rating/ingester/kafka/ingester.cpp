#include <spdlog/spdlog.h>
#include "rating/ingester/kafka/ingester.h"

#include <cpprest/json.h>
#include "cppcoro/on_scope_exit.hpp"

#include "config/config.h"
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
            // json array 형식으로 데이터가 오지 않았다. (한개의 데이터)
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
            { "enable.auto.commit", false },
            { "auto.offset.reset", "earliest" }
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

    Ingester::Ingester(const std::shared_ptr<config::Config>& config)
    {
        auto kafkaHost = config->get<std::string>("application.kafka.consumer.bootstrap-servers");
        auto groupId = config->get<std::string>("application.kafka.consumer.group-id");
        const std::string topic = "ratings";

        const Configuration configuration = {
            { "metadata.broker.list", kafkaHost},
            { "group.id", groupId }
        };
        spdlog::info("Create new Ingester");
        spdlog::info("broker.list: {}", kafkaHost);
        spdlog::info("group.id: {}", groupId);
        spdlog::info("topic: {}", topic);

        consumer_ = std::make_unique<Consumer>(configuration);
        consumer_->subscribe({topic});
    }

    void Ingester::Poll(RatingEventSet& events)
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
                        std::copy(std::begin(r), std::end(r),
                                    std::inserter(events, std::end(events)));
                    });
                consumer_->commit(msg);
            }
        }
    }

    Ingester::Ingester(std::unique_ptr<cppkafka::Consumer> consumer) noexcept
        : consumer_(std::move(consumer))
    {

    }


}