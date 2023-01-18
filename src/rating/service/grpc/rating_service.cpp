#include <spdlog/spdlog.h>
#include "rating/service/grpc/rating_service.h"
#include "rating/controller/controller.h"
#include "rating/repository/memory/memory.h"
#include "rating/repository/postgresql/postgresql.h"

using grpc::Status;
using movie::GetAggregatedRatingRequest;
using movie::GetAggregatedRatingResponse;
using movie::PutRatingRequest;
using movie::PutRatingResponse;

#include <condition_variable_any2.hpp>

class RatingServiceImpl final : public movie::RatingService::Service
{
public:
    explicit RatingServiceImpl(const std::shared_ptr<rating::controller::Controller>& controller)
        : controller_(controller)
    {}
private:
    Status GetAggregatedRating(
        ServerContext* context,
        const GetAggregatedRatingRequest* request,
        GetAggregatedRatingResponse* response) override
    {
        if (request == nullptr || 
            request->record_id().empty() ||
            request->record_type().empty())
        {
            return Status(
                grpc::StatusCode::INVALID_ARGUMENT,
                "request is null or empty id or type");
        }
        auto v = controller_->Get(request->record_id(), request->record_type());
        if (!v.has_value() || v.value().empty())
        {
            spdlog::error(v.error());
            return Status(
                grpc::StatusCode::NOT_FOUND,
                "not found");
        }
        double sum = 0.0;
        // rating 평균을 계산한다.
        for (const auto& e : v.value())
        {
            sum += e.ratingValue;
        }
        double avg = (sum / v.value().size());

        response->set_rating_value(avg);

        return Status::OK;
    }

    Status PutRating(
        ServerContext* context,
        const PutRatingRequest* request,
        PutRatingResponse* response) override
    {
        if (request == nullptr ||
            request->record_id().empty() ||
            request->user_id().empty())
        {
            return Status(
                grpc::StatusCode::INVALID_ARGUMENT,
                "request is null or empty record id or user id");
        }
        rating::model::Rating result;
        result.recordId = request->record_id();
        result.recordType = request->record_type();
        result.userId = request->user_id();
        result.ratingValue = request->rating_value();
        spdlog::info("[RatingService::PutRating] record ID: {}, record type: {}, user ID: {}, rating: {}",
            request->record_id(), request->record_type(), request->user_id(), request->rating_value());

        controller_->Put(
            request->record_id(), 
            request->record_type(), 
            result);

        return Status::OK;
    }
private:
    std::shared_ptr<rating::controller::Controller> controller_;
};

namespace rating::service::grpc
{
    RatingService::RatingService(std::shared_ptr<rating::controller::Controller> controller)
        : controller_(controller)
    {
        ::grpc::EnableDefaultHealthCheckService(true);
        ::grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    }

    RatingService::~RatingService()
    {
        stop();
    }
    
    void RatingService::start(const std::string& addr)
    {
        spdlog::info("[RatingService] start service");
        spdlog::info("[RatingService] Listening for requests at: {}", addr);
        controller_->StartIngestion();
        ServerBuilder builder;
        RatingServiceImpl service(controller_);
        builder.AddListeningPort(addr, ::grpc::InsecureServerCredentials());
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

    void RatingService::stop()
    {
        spdlog::info("[RatingService] stop service");
        thread_.request_stop();
        if (thread_.joinable())
        {
            thread_.join();
        }
    }
}