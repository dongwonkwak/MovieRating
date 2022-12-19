#pragma once

#include "common/expected.h"
#include <cpprest/json.h>

#include <string>
#include <vector>

namespace rating::model
{
    struct Rating
    {
        std::string recordId;       // key1
        std::string recordType;     // key2
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

    web::json::value AsJSON(const common::expected<std::vector<common::expected<Rating>>>& rating);
}