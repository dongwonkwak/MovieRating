#include "movie/service/http/movie_service.h"
#include "movie/controller/controller.h"

namespace movie::service::http
{
    MovieService::MovieService(const std::shared_ptr<movie::controller::Controller>& controller, utility::string_t url)
        : listener_(url)
        , controller_(controller)
    {
        listener_.support(methods::GET, std::bind(&MovieService::handle_get, this, std::placeholders::_1));
    }

    void MovieService::handle_get(http_request message)
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
            auto details = controller_->Get(found->second);
            ucout << details->AsJSON() << std::endl;
            message.reply(status_codes::OK);
        }
    }
}