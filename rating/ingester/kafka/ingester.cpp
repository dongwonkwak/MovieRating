#include "rating/ingester/kafka/ingester.h"
using namespace kafka;
using namespace kafka::clients;
using namespace kafka::clients::consumer;

#include <cpprest/json.h>


namespace rating::ingester::kafka
{
    static auto parseRatingEvents(const std::string& line)
        -> common::expected<std::vector<rating::model::RatingEvent>>
    {
        using namespace rating::model;
        std::vector<RatingEvent> res;

        try
        {
            auto obj = web::json::value::parse(line);
            if (!obj.is_array())
            {
                res.emplace_back(RatingEvent::FromJSON(obj));
                return res;
            }
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

////////////////////////
    std::shared_ptr<Ingester> Ingester::NewIngester(
            const std::string& addr,
            const std::string& groupId,
            const std::string& topic)
    {
        const Properties props({
            {"bootstrap.servers", {addr}},
            {"group.id", {groupId}},
        });
        auto consumer = std::make_unique<KafkaConsumer>(props);
        consumer->subscribe({topic});
        auto ingester = std::shared_ptr<Ingester>(new Ingester(std::move(consumer)));
        return ingester;
    }

    auto Ingester::Poll()
        -> common::expected<std::vector<rating::model::RatingEvent>>
    {
        auto records = consumer_->poll(std::chrono::milliseconds(100));
        if (records.size() == 0)
        {
            return common::unexpected{"empty"};
        }
        std::cout << "get records\n";

        std::vector<rating::model::RatingEvent> res;
        for (const auto& record : records)
        {
            if (record.value().size() == 0) continue;
            if (record.error()) continue;

            std::cout << record.value().toString() << std::endl;
            parseRatingEvents(record.value().toString())
                .map([&](const auto& r) {
                    res.insert(res.begin(), r.begin(), r.end());
                });
        } 

        return res;
    }

    Ingester::Ingester(std::unique_ptr<::kafka::clients::consumer::KafkaConsumer> consumer) noexcept
        : consumer_(std::move(consumer))
    {

    }


}