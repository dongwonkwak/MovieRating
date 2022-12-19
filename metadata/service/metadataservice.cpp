#include "service/metadataservice.h"
#include "controller/controller.h"
#include "repository/repository.h"

#include <iostream>

namespace metadata::service
{
    MetadataService::MetadataService(std::unique_ptr<metadata::controller::Controller> controller, utility::string_t url)
        : controller_(std::move(controller))
        , listener_(url)
    {
        listener_.support(methods::GET, std::bind(&MetadataService::handle_get, this, std::placeholders::_1));
        //listener_.support(methods::PUT, std::bind(&MetadataService::handle_put, this, std::placeholders::_1));
    }

    void MetadataService::handle_get(http_request message)
    {
        std::map<utility::string_t, utility::string_t> query =
            uri::split_query(uri::decode(message.request_uri().query()));
        auto found = query.find("id");
        if (found == query.end())
        {
            message.reply(status_codes::NotFound);
        }
        else
        {
            ucout << found->second << std::endl;
            auto id = controller_->Get(found->second);
            if (id)
            {
                message.reply(status_codes::OK, id.value().AsJSON());
            }
            else
            {
                message.reply(status_codes::NotFound);
            }
        }

    }

    void MetadataService::handle_put(http_request message)
    {

    }
}