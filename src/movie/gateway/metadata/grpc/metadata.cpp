#include "spdlog/spdlog.h"
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
    MetadataServiceClient(const std::shared_ptr<Channel>& channel)
        : stub_(movie::MetadataService::NewStub(channel))
    {
    
    }

    auto Get(const std::string& id) 
        -> common::expected<movie::Metadata>
    {
        spdlog::debug("MetadataServiceClient got Get request - id: {}", id);
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
    MetadataGateway::MetadataGateway(const std::shared_ptr<discovery::Registry>& registry)
       : registry_(registry)
    {

    }

    MetadataGateway::~MetadataGateway()
    {

    }

    auto MetadataGateway::Get(const std::string& id) 
        -> common::expected<::metadata::model::Metadata>
    {
        if (id.empty())
        {
            return common::unexpected{"id is empty"};
        }
        // 서비스 디스커버리
        return grpcutil::ServiceConnection("metadata", registry_)
            .and_then([=](auto conn) {
                MetadataServiceClient client(conn);
                return client.Get(id);
            })
            .map([=](auto resp) {
                return ::metadata::model::Metadata::MetadataFromProto(resp);
            });
    }
}