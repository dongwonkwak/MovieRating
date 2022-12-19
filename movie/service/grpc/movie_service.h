#pragma once


#include "movie.pb.h"
#include "movie.grpc.pb.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;


#include <memory>
#include <string>

namespace movie::controller
{
    class Controller;
}

namespace movie::service::grpc
{
    class MovieService
    {
    public:
        MovieService(std::unique_ptr<controller::Controller> controller, const std::string& addr);
        ~MovieService();
        void start();
        void stop();
    private:
        std::shared_ptr<controller::Controller> controller_;
        std::unique_ptr<Server> server_;
        std::string addr_;
    };
}