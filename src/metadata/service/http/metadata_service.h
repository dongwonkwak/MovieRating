#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
using namespace web;
using namespace http;
using namespace utility;
using namespace web::http::experimental::listener;

#include <memory>

namespace metadata::controller
{
    class Controller;
}

namespace metadata::service::http
{
    class MetadataService
    {
    public:
        MetadataService(const std::shared_ptr<controller::Controller>& controller, utility::string_t url);
        pplx::task<void> start() { return listener_.open(); }
        pplx::task<void> stop() { return listener_.close(); }
    private:
        void handle_get(http_request message);
        void handle_put(http_request message);
    private:
        http_listener listener_;
        std::shared_ptr<metadata::controller::Controller> controller_;
    };
}