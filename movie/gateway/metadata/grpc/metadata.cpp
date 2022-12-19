#include <fmt/core.h>
#include <fmt/std.h>

#include "movie/gateway/metadata/grpc/metadata.h"
#include "movie/grpcutil/grpcutil.h"

#include <grpcpp/grpcpp.h>
#include "movie.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using movie::GetMetadataRequest;
using movie::GetMetadataResponse;

class MetadataServiceClient
{
public:
    MetadataServiceClient(std::shared_ptr<Channel> channel)
        : stub_(movie::MetadataService::NewStub(channel))
    {
    
    }

    auto Get(const std::string& id) 
        -> common::expected<movie::Metadata>
    {

        ClientContext context;
        GetMetadataRequest request;
        GetMetadataResponse response;

        request.set_movie_id(id);
        Status status = stub_->GetMetadata(&context, request, &response);
        if (status.ok())
        {
            return response.metadata();
        }

        return common::unexpected{fmt::format("error code: {}, {}", 
            status.error_code(), status.error_message())};
    }
private:
    std::unique_ptr<movie::MetadataService::Stub> stub_;
};

namespace movie::gateway::metadata::grpc
{
    MetadataGateway::MetadataGateway(std::shared_ptr<discovery::Registry> registry) noexcept
       : registry_(registry)
    {

    }

    MetadataGateway::MetadataGateway(MetadataGateway&& other)
        : registry_(std::move(other.registry_))
    {

    }

    MetadataGateway& MetadataGateway::operator=(MetadataGateway&& other)
    {
        registry_ = std::move(other.registry_);
        return *this;
    }

    MetadataGateway::~MetadataGateway() noexcept
    {

    }

    auto MetadataGateway::Get(const std::string& id) 
        -> common::expected<::metadata::model::Metadata>
    {
        if (id.empty())
        {
            return common::unexpected{"id is empty"};
        }

        auto conn = grpcutil::ServiceConnection("metadata", registry_);
        if (!conn.has_value())
        {
            return common::unexpected("there is no metadata service");
        }

        MetadataServiceClient client(conn.value());
        auto resp = client.Get(id);
        if (!resp.has_value())
        {
            return common::unexpected("not found");
        }

        return ::metadata::model::Metadata::MetadataFromProto(resp.value());
    }
}