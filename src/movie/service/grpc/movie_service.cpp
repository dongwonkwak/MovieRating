#include <spdlog/spdlog.h>
#include "movie/service/grpc/movie_service.h"
#include "movie/controller/controller.h"
#include "movie/grpcutil/grpcutil.h"
#include "movie.grpc.pb.h"

using grpc::Status;
using movie::GetMovieDetailsRequest;
using movie::GetMovieDetailsResponse;

#include <condition_variable_any2.hpp>


class MovieServiceImpl final : public movie::MovieService::Service
{
public:
    explicit MovieServiceImpl(const std::shared_ptr<movie::controller::Controller>& controller)
        : controller_(controller)
    {}
private:
    Status GetMovieDetails(
        ServerContext* context,
        const GetMovieDetailsRequest* request,
        GetMovieDetailsResponse* response) override
    {
        if (request == nullptr || request->movie_id().empty())
        {
            return Status(
                grpc::StatusCode::INVALID_ARGUMENT, 
                "request is null or empty id");
        }
        auto details = controller_->Get(request->movie_id());
        if (!details.has_value())
        {
            return Status(
                grpc::StatusCode::NOT_FOUND,
                "not found");
        }
                
        response->mutable_movie_details()->CopyFrom(details->MovieDetailsToProto());

        return Status::OK;
    }

private:
    std::shared_ptr<movie::controller::Controller> controller_;
};

namespace movie::service::grpc
{
    MovieService::MovieService(const std::shared_ptr<controller::Controller>& controller, const std::string &addr)
        : controller_(controller)
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
        spdlog::info("GrpcService is starting...");
        ServerBuilder builder;
        MovieServiceImpl service(controller_);
        builder.AddListeningPort(addr_, ::grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        server_ = std::unique_ptr<Server>(builder.BuildAndStart());
        thread_ = std::jthread([this](std::stop_token token){
            std::mutex mutex;
            std::unique_lock lock(mutex);
            std::condition_variable_any2().wait(lock, token,
                        [&token] { return false; });
            server_->Shutdown();
        });

        server_->Wait();
    }

    void MovieService::stop()
    {
        spdlog::info("GrpcService is stopped");
        thread_.request_stop();
        if (thread_.joinable())
        {
            thread_.join();
        }
    }
}