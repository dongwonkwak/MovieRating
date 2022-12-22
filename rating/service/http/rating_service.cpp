#include "rating/service/http/rating_service.h"
#include "rating/controller/controller.h"
#include "rating/repository/memory.h"


namespace rating::service::http
{
    HttpService::HttpService(std::unique_ptr<rating::controller::Controller> controller, utility::string_t url)
        : listener_(url)
        , controller_(std::move(controller))
    {
        listener_.support(methods::GET, std::bind(&HttpService::handle_get, this, std::placeholders::_1));
        listener_.support(methods::PUT, std::bind(&HttpService::handle_put, this, std::placeholders::_1));
    }

    pplx::task<void> HttpService::start()
    {
        controller_->StartIngestion();
        return listener_.open();
    }

    void HttpService::handle_get(http_request message)
    {
        std::map<utility::string_t, utility::string_t> query =
            uri::split_query(uri::decode(message.relative_uri().query()));
        auto found_id = query.find("id");
        auto found_type = query.find("type");
        if (found_id == query.end() || found_type == query.end())
        {
            message.reply(status_codes::NotFound);
            return;
        }

        auto v = controller_->Get(found_id->second, found_type->second);
        if (!v.has_value())
        {
            message.reply(status_codes::OK, "0");
            return;
        }
        
        double sum = 0.0;
        for (const auto& e : v.value())
        {
            sum += e->ratingValue;
        }
        double avg = (sum / v.value().size());

        message.reply(status_codes::OK, std::to_string(avg));
    }

    void HttpService::handle_put(http_request message)
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
        model::Rating result;
        result.recordId = found_id->second;
        result.recordType = found_type->second;
        result.userId = found_userid->second;
        result.ratingValue = stod(found_value->second);

        controller_->Put(found_id->second, found_type->second, result);
        message.reply(status_codes::OK);
    }
}