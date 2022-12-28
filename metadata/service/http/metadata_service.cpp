#include "spdlog/spdlog.h"
#include "metadata/service/http/metadata_service.h"
#include "metadata/controller/controller.h"
#include "metadata/repository/repository.h"

#include <iostream>

namespace metadata::service::http
{
    MetadataService::MetadataService(const std::shared_ptr<metadata::controller::Controller>& controller, utility::string_t url)
        : controller_(controller)
        , listener_(url)
    {
        listener_.support(methods::GET, std::bind(&MetadataService::handle_get, this, std::placeholders::_1));
        //listener_.support(methods::PUT, std::bind(&MetadataService::handle_put, this, std::placeholders::_1));
    }

    void MetadataService::handle_get(http_request message)
    {
        std::map<utility::string_t, utility::string_t> query =
            uri::split_query(uri::decode(message.request_uri().query()));

        spdlog::debug("got request: {}", message.relative_uri().query().c_str());
        
        auto found = query.find("id");
        if (found == query.end())
        {
            spdlog::debug("empty id");
            message.reply(status_codes::NotFound);
        }
        else
        {
            spdlog::debug("id: {}", found->second);
            controller_->Get(found->second)
                .map([=](auto id) {
                    spdlog::debug("found!");
                    message.reply(status_codes::OK, id.AsJSON());
                })
                .map_error([=](auto e) {
                    spdlog::debug("not found");
                    message.reply(status_codes::NotFound);
                });
        }

    }

    void MetadataService::handle_put(http_request message)
    {

    }
}