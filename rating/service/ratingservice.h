#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
using namespace web;
using namespace http;
using namespace utility;
using namespace web::http::experimental::listener;

#include <memory>

namespace rating::controller
{
    class Controller;
}
namespace rating::service
{
    class RatingService
    {
    public:
        RatingService(std::unique_ptr<rating::controller::Controller> controller, utility::string_t url);
        pplx::task<void> start() { return listener_.open(); }
        pplx::task<void> stop() { return listener_.close(); }
    private:
        void handle_get(http_request message);
        void handle_put(http_request message);
    private:
        http_listener listener_;
        std::unique_ptr<rating::controller::Controller> controller_;
    };
}