#include "test_utils.h"
#include <cpprest/json.h>

auto parseRatings(const std::string& inp) -> std::vector<rating::model::RatingEvent>
{
    auto obj = web::json::value::parse(inp);
    std::vector<rating::model::RatingEvent> res;
    if (!obj.is_array())
    {
        res.push_back(rating::model::RatingEvent::FromJSON(obj));
        return res;
    }

    auto objs = obj.as_array();
    for (const auto& v : objs)
    {
        res.push_back(rating::model::RatingEvent::FromJSON(v));
    }
    return res;
}