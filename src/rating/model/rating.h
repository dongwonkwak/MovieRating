#pragma once

#include "common/expected.h"
#include <cpprest/json.h>

#include <string>
#include <vector>
#include <unordered_set>
namespace rating::model
{
    // key = recordId + recordType
    struct Rating
    {
        std::string recordId;
        std::string recordType;
        std::string userId;
        double ratingValue;

        web::json::value AsJSON() const
        {
            auto res = web::json::value::object();
            res["recordId"] = web::json::value::string(recordId);
            res["recordType"] = web::json::value::string(recordType);
            res["userId"] = web::json::value::string(userId);
            res["value"] = web::json::value::number(ratingValue);

            return res;
        }

        static Rating FromJSON(const web::json::object& object)
        {
            Rating result;
            auto id = object.find("recordId");
            if (id == object.end())
            {
                throw web::json::json_exception(U("recordId key not found"));
            }
            auto type = object.find("recordType");
            if (type == object.end())
            {
                throw web::json::json_exception(U("recordType key not found"));
            }
            auto userId = object.find("userId");
            if (userId == object.end())
            {
                throw web::json::json_exception(U("userId key not found"));
            }
            auto value = object.find("value");
            if (value == object.end())
            {
                throw web::json::json_exception(U("value key not found"));
            }

            result.recordId = id->second.as_string();
            result.recordType = type->second.as_string();
            result.userId = userId->second.as_string();
            result.ratingValue = userId->second.as_double();

            return result;
        }
    };

    constexpr std::string_view RecordTypeMovie = "movie";

    constexpr std::string_view RatingEventTypePut = "put";
    constexpr std::string_view RatingEventTypeDelete = "delete";

    inline web::json::value AsJSON(const common::expected<std::vector<common::expected<Rating>>>& rating)
    {
        auto res = web::json::value::array(rating->size());
        int index = 0;
        for (const auto r : rating.value())
        {
            res[index++] = r->AsJSON();
        }
        return res;
    }

    // key = recordId + recordType
    struct RatingEvent
    {
        std::string userId;
        std::string recordId;
        std::string recordType;
        int value;
        std::string providerId;
        std::string eventType;

        static RatingEvent FromJSON(web::json::value object)
        {
            RatingEvent result;
            result.userId = object["userId"].as_string();
            result.recordId = object["recordId"].as_string();
            result.recordType = object["recordType"].as_string();
            result.value = object["value"].as_integer();
            result.providerId = object["providerId"].as_string();
            result.eventType = object["eventType"].as_string();

            return result;
        }

        web::json::value AsJSON() const
        {
            auto res = web::json::value::object();
            res["userId"] = web::json::value::string(userId);
            res["recordId"] = web::json::value::string(recordId);
            res["recordType"] = web::json::value::string(recordType);
            res["value"] = web::json::value::number(value);
            res["providerId"] = web::json::value::string(providerId);
            res["eventType"] = web::json::value::string(eventType);

            return res;
        }

        bool operator==(const RatingEvent& other) const
        {
            return ((recordId == other.recordId) && (recordType == other.recordType));
        }

        bool operator!=(const RatingEvent& other) const
        {
            return !(*this == other);
        }
    };

    struct RatingEventHash
    {
        size_t operator()(const RatingEvent& e) const
        {
            return (std::hash<std::string>()(e.recordId)) ^ (std::hash<std::string>()(e.recordType));
        }
    };
}

using RatingEventSet = std::unordered_set<rating::model::RatingEvent, rating::model::RatingEventHash>;