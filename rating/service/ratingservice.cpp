#include "service/ratingservice.h"
#include "controller/controller.h"
#include "repository/memory.h"


namespace rating
{
    RatingService::RatingService(std::unique_ptr<Controller>&& controller, utility::string_t url)
        : listener_(url)
        , controller_(std::move(controller))
    {
        listener_.support(methods::GET, std::bind(&RatingService::handle_get, this, std::placeholders::_1));
        listener_.support(methods::PUT, std::bind(&RatingService::handle_put, this, std::placeholders::_1));
    }

    void RatingService::handle_get(http_request message)
    {
        std::map<utility::string_t, utility::string_t> query =
            uri::split_query(uri::decode(message.request_uri().query()));
        auto found_id = query.find("id");
        auto found_type = query.find("type");
        if (found_id == query.end() || found_type == query.end())
        {
            message.reply(status_codes::NotFound);
            return;
        }

        auto v = controller_->Get(found_id->second, found_type->second);
        double sum = 0.0;
        for (const auto& e : v.value())
        {
            sum += e->ratingValue;
        }
        double avg = (sum / v.value().size());

        message.reply(status_codes::OK, std::to_string(avg));
    }

    void RatingService::handle_put(http_request message)
    {
        std::map<utility::string_t, utility::string_t> query =
            uri::split_query(uri::decode(message.request_uri().query()));
        auto found_id = query.find("id");
        auto found_type = query.find("type");
        auto found_userid = query.find("userId");
        auto found_value = query.find("value");
        if (found_id == query.end() || found_type == query.end() ||
            found_userid == query.end() || found_value == query.end())
        {
            message.reply(status_codes::NotFound);
            return;
        }
        Rating result;
        result.recordId = found_id->second;
        result.recordType = found_type->second;
        result.userId = found_userid->second;
        result.ratingValue = stod(found_value->second);

        controller_->Put(found_id->second, found_type->second, result);
        message.reply(status_codes::OK);
    }
}