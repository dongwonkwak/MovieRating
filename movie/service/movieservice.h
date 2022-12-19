#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
using namespace web;
using namespace http;
using namespace utility;
using namespace web::http::experimental::listener;

#include <memory>

namespace movie::controller
{
    class Controller;
}
namespace movie::service
{
    class MovieService
    {
    public:
        explicit MovieService(std::unique_ptr<movie::controller::Controller> controller, utility::string_t url);
        pplx::task<void> start() { return listener_.open(); }
        pplx::task<void> stop() { return listener_.close(); }
    private:
        void handle_get(http_request message);
    private:
        http_listener listener_;
        std::unique_ptr<movie::controller::Controller> controller_;
    };
}