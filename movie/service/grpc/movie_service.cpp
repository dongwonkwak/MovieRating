#include "movie/service/grpc/movie_service.h"
#include "movie/controller/controller.h"
#include "movie/grpcutil/grpcutil.h"

namespace movie::service::grpc
{
    MovieService::MovieService(std::unique_ptr<controller::Controller> controller, const std::string &addr)
        : controller_(std::move(controller))
        , addr_(addr)
    {
        ::grpc::EnableDefaultHealthCheckService(true);
        ::grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    }

    MovieService::~MovieService()
    {
        stop();
    }
    
    void MovieService::start()
    {
        std::cout << "GrpcService start\n";
        ServerBuilder builder;
        //MovieService service(controller_);
        builder.AddListeningPort(addr_, ::grpc::InsecureServerCredentials());
        //builder.RegisterService(&service);

        server_ = std::unique_ptr<Server>(builder.BuildAndStart());

        server_->Wait();
    }

    void MovieService::stop()
    {
        if (server_)
        {
            std::cout << "stop!!\n";
            server_->Shutdown();
            server_.release();
        }
    }
}