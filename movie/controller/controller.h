#pragma once

#include "common/expected.h"
#include "metadata/model/metadata.h"
#include "movie/gateway/gateway.h"
#include "movie/model/movie.h"

#include <string>
#include <memory>

namespace movie::controller
{
    class Controller
    {
    public:
        explicit Controller(
            std::unique_ptr<gateway::IRatingGateway> ratingGateway, 
            std::unique_ptr<gateway::IMetadataGateway> metadataGateway);
        auto Get(const std::string& id) -> common::expected<movie::model::MovieDetails>;
    private:
        std::unique_ptr<gateway::IRatingGateway> ratingGateway_;
        std::unique_ptr<gateway::IMetadataGateway> metadataGateway_;
    };
}