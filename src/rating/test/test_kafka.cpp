#include "range/v3/all.hpp"

#include <cpprest/json.h>

#include "rating/model/rating.h"
using namespace rating;

#include "common/expected.h"
#include <iostream>
#include <vector>
using namespace common;

#include <cppcoro/task.hpp>
#include <cppcoro/generator.hpp>

#include "kafka/KafkaProducer.h"
using namespace kafka;
using namespace kafka::clients;
using namespace kafka::clients::producer;

using Result = expected<std::vector<rating::model::RatingEvent>>;

auto openFile(const std::string& fileName) -> expected<std::ifstream>
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        return unexpected{"error while opening file"};
    }
    return file;
}

auto readLine(std::ifstream& file) -> cppcoro::generator<std::string>
{
    std::string line;
    while (getline(file, line))
    {
        co_yield line;
    }
    co_return;
}


auto parseRatings(const std::string& line)
    -> cppcoro::generator<expected<rating::model::RatingEvent>>
{
    auto obj = web::json::value::parse(line);
    
    if (!obj.is_array())
    {
        co_yield rating::model::RatingEvent::FromJSON(obj);
    }

    auto objs = obj.as_array();
    for (const auto& v : objs)
    {
        ucout << v << std::endl;
        co_yield rating::model::RatingEvent::FromJSON(v);
    }
}


auto readRatings(const std::string& path) 
    -> cppcoro::generator<expected<rating::model::RatingEvent>>
{
    auto file = openFile(path);
    if (!file)
    {
        co_yield unexpected{"can't open file"};
    }
    using namespace ranges::views;
    auto g = readLine(file.value());

    auto lines = ranges::views::common(g) |
        filter([](const expected<std::string>& e) { return e.has_value(); }) |
        transform([](const expected<std::string>& e) { return e.value<std::string>(); });

    for (const auto& line : lines)                  
    {
        auto g2 = parseRatings(line);
        for (const auto& rating : ranges::views::common(g2)
                                | filter([](const expected<rating::model::RatingEvent>& e) { return e.has_value(); })
                                | transform([](const expected<rating::model::RatingEvent>& e) 
                                    { 
                                        return e.value<rating::model::RatingEvent>();
                                    }))
        {
            co_yield rating;
        }
    }
}

auto readRatingHelper(cppcoro::generator<expected<rating::model::RatingEvent>> generator) 
    -> std::vector<rating::model::RatingEvent>
{
    using namespace ranges::views;
    using namespace rating::model;

    auto ratings = ranges::views::common(generator) |
        filter([](const expected<RatingEvent>& e) { return e.has_value(); }) |
        transform([](const expected<RatingEvent>& e) { return e.value<RatingEvent>(); });

    std::vector<RatingEvent> ratingVec{};
    ranges::copy(ratings, std::back_inserter(ratingVec));

    return ratingVec;
}

Result readRatingEvent(const std::string& filename)
{
    return readRatingHelper(readRatings(filename));
}

auto produceRatingEvents(
    const std::string& topic, 
    KafkaProducer& producer, 
    const std::vector<rating::model::RatingEvent>& events)
    -> bool
{
    auto res = web::json::value::array(events.size());
    for (size_t i = 0; i < events.size(); ++i)
    {
        res[i] = events[i].AsJSON();
    }
    auto v = res.serialize();

    try
    {
        auto record = ProducerRecord(topic, NullKey, Value(v.c_str(), v.size()));
        producer.send(record,
                // The delivery report handler
                [](const RecordMetadata& metadata, const Error& error) {
                    if (!error) {
                        std::cout << "% Message delivered: " << metadata.toString() << std::endl;
                    } else {
                        std::cerr << "% Message delivery failed: " << error.message() << std::endl;
                    }
                },
                // The memory block given by record.value() would be copied
                KafkaProducer::SendOption::ToCopyRecordValue);
    }
    catch (const KafkaException& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return true;
}

int main()
{
    std::string filename = "./data/ratingsdata.json";
    auto result = readRatingEvent(filename);
    
    const std::string brokers = "127.0.0.1:9092";
    const std::string topic = "ratings";
    const std::string groupId = "movie-rating";

    try
    {
        const Properties props({
            {"bootstrap.servers", {brokers}},
            {"group.id", {groupId}},
        });

        KafkaProducer producer(props);
        produceRatingEvents(topic, producer, result.value());
    }
    catch (const KafkaException& e)
    {
        std::cerr << "error!!\n";
        std::cerr << e.what() << std::endl;
    }


    return 0;
}