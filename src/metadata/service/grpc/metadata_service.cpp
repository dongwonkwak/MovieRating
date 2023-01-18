#include "spdlog/spdlog.h"
#include "metadata/service/grpc/metadata_service.h"
#include "metadata/controller/controller.h"
#include "metadata/repository/repository.h"

using grpc::Status;
using movie::GetMetadataRequest;
using movie::GetMetadataResponse;
using movie::PutMetadataRequest;
using movie::PutMetadataResponse;

#include <condition_variable_any2.hpp>

class MetadataServiceImpl final : public movie::MetadataService::Service
{
public:
    explicit MetadataServiceImpl(const std::shared_ptr<metadata::controller::Controller>& controller)
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

        return controller_->Get(request->movie_id())
            .map([=](auto id) {
                auto result = id.MetadataToProto();
                response->mutable_metadata()->CopyFrom(result);

                return Status::OK;
            })
            .map_error([=](auto e) {
                spdlog::error("[MetadataServiceImpl::GetMetadata] {} not found", 
                    request->movie_id());
            })
            .value_or(Status(
                    grpc::StatusCode::NOT_FOUND,
                    "not found"));
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

    MetadataService::MetadataService(const std::shared_ptr<controller::Controller>& controller, const std::string &addr)
        : controller_(controller)
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
        spdlog::info("Metadata GrpcService starts...");
        ServerBuilder builder;
        MetadataServiceImpl service(controller_);
        builder.AddListeningPort(addr_, ::grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        server_ = std::unique_ptr<Server>(builder.BuildAndStart());
        thread_ = std::jthread([this](std::stop_token token){
            std::mutex mutex;
            std::unique_lock lock(mutex);
            std::condition_variable_any2().wait(lock, token,
                        [] { return false; });
            server_->Shutdown();
        });

        server_->Wait();
    }

    void MetadataService::stop()
    {
        spdlog::info("[Metadata Service] stop service");
        thread_.request_stop();
        if (thread_.joinable())
        {
            thread_.join();
        }
    }
}