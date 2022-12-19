#include "movie/service/grpc/movie_service.h"
#include "movie/controller/controller.h"
#include "movie/grpcutil/grpcutil.h"
#include "movie.grpc.pb.h"

using grpc::Status;
using movie::GetMovieDetailsRequest;
using movie::GetMovieDetailsResponse;

class MovieServiceImpl final : public movie::MovieService::Service
{
public:
    explicit MovieServiceImpl(std::shared_ptr<movie::controller::Controller> controller)
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
        movie::MovieDetails result;
        auto data = details->metadata.MetadataToProto();
        result.set_allocated_metadata(&data);
        result.set_rating(details->rating);
        response->set_allocated_movie_details(&result);

        return Status::OK;
    }

private:
    std::shared_ptr<movie::controller::Controller> controller_;
};

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
        MovieServiceImpl service(controller_);
        builder.AddListeningPort(addr_, ::grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

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