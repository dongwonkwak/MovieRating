#pragma once

#include "movie.pb.h"
#include "movie.grpc.pb.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;

#include <jthread.hpp>

#include <memory>
#include <string>

namespace metadata::controller
{
    class Controller;
}

namespace metadata::service::grpc
{
    class MetadataService
    {
    public:
        /**
         * @brief Construct a new Metadata Service object
         * 
         * @param controller 비지니스 로직
         * @param addr Grpc서비스에서 사용하는 네트워크 주소
         */
        MetadataService(const std::shared_ptr<controller::Controller>& controller, const std::string& addr);
        ~MetadataService();
        void start();
        void stop();
    private:
        std::shared_ptr<metadata::controller::Controller> controller_;
        std::unique_ptr<Server> server_;
        std::string addr_;
        std::jthread thread_;
    };
}