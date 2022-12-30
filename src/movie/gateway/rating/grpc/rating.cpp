#include <fmt/core.h>
#include <fmt/std.h>

#include "movie/gateway/rating/grpc/rating.h"
#include "movie/grpcutil/grpcutil.h"

#include <grpcpp/grpcpp.h>
#include "movie.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using movie::GetAggregatedRatingRequest;
using movie::GetAggregatedRatingResponse;
using movie::PutRatingRequest;
using movie::PutRatingResponse;

class RatingServiceClient
{
public:
    RatingServiceClient(std::shared_ptr<Channel> channel)
        : stub_(movie::RatingService::NewStub(channel))
    {}

    auto GetAggretatedRating(const std::string& recordID, const std::string& recordType)
        -> common::expected<double>
    {
        ClientContext context;
        GetAggregatedRatingRequest request;
        GetAggregatedRatingResponse response;
        request.set_record_id(recordID);
        request.set_record_type(recordType);

        auto status = stub_->GetAggregatedRating(&context, request, &response);

        if (status.ok())
        {
            return response.rating_value();
        }

        return common::unexpected{fmt::format("error code: {}, {}", 
            status.error_code(), status.error_message())};
    }
private:
    std::unique_ptr<movie::RatingService::Stub> stub_;
};

namespace movie::gateway::rating::grpc
{
    RatingGateway::RatingGateway(const std::shared_ptr<discovery::Registry>& registry) noexcept
        : registry_(registry)
    {

    }

    auto RatingGateway::GetAggretatedRating(
                        const std::string& recordID, 
                        const std::string& recordType)
        -> common::expected<double>
    {
        if (recordID.empty() || recordType.empty())
        {
            return common::unexpected{"record ID or record type is empty"};
        }

        return grpcutil::ServiceConnection("rating", registry_)
                .map([=](auto conn) {
                    RatingServiceClient client(conn);
                    return client.GetAggretatedRating(recordID, recordType);
                })
                .map_error([](auto e) { return common::unexpected{"there is no rating service"}; })
                .value();
    }

    void RatingGateway::PutRating(
                    const std::string& recordID, 
                    const std::string& recordType, 
                    const ::rating::model::Rating& rating)
    {
    }
}