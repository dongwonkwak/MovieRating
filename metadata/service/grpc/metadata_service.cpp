#include "metadata/service/grpc/metadata_service.h"
#include "metadata/controller/controller.h"
#include "metadata/repository/repository.h"

using grpc::Status;
using movie::GetMetadataRequest;
using movie::GetMetadataResponse;
using movie::PutMetadataRequest;
using movie::PutMetadataResponse;

#include <iostream>

class MetadataServiceImpl final : public movie::MetadataService::Service
{
public:
    explicit MetadataServiceImpl(std::shared_ptr<metadata::controller::Controller> controller)
        : controller_(controller)
    {

    }
private:
    Status GetMetadata(
        ServerContext* context, 
        const GetMetadataRequest* request,
        GetMetadataResponse* response) override
    {
        if (request == nullptr || request->movie_id().empty())
        {
            return Status(
                grpc::StatusCode::INVALID_ARGUMENT, 
                "request is null or empty id");
        }
        auto id = controller_->Get(request->movie_id());
        if (!id.has_value())
        {
            return Status(
                grpc::StatusCode::NOT_FOUND,
                "not found");
        }

        auto result = id->MetadataToProto();
        response->mutable_metadata()->CopyFrom(result);

        return Status::OK;
    }

    Status PutMetadata(
        ServerContext* context,
        const PutMetadataRequest* request,
        PutMetadataResponse* response) override
    {
        return Status::OK;
    }

    std::shared_ptr<metadata::controller::Controller> controller_;
};



namespace metadata::service::grpc
{

    MetadataService::MetadataService(std::unique_ptr<controller::Controller> controller, const std::string &addr)
        : controller_(std::move(controller))
        , addr_(addr)
    {
        ::grpc::EnableDefaultHealthCheckService(true);
        ::grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    }

    MetadataService::~MetadataService()
    {
        stop();
    }
    
    void MetadataService::start()
    {
        std::cout << "GrpcService start\n";
        ServerBuilder builder;
        MetadataServiceImpl service(controller_);
        builder.AddListeningPort(addr_, ::grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        server_ = std::unique_ptr<Server>(builder.BuildAndStart());

        server_->Wait();
    }

    void MetadataService::stop()
    {
        if (server_)
        {
            std::cout << "stop!!\n";
            server_->Shutdown();
            server_.release();
        }
    }
}