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
            const std::shared_ptr<gateway::IRatingGateway>& ratingGateway, 
            const std::shared_ptr<gateway::IMetadataGateway>& metadataGateway);
        auto Get(const std::string& id) -> common::expected<movie::model::MovieDetails>;
    private:
        std::shared_ptr<gateway::IRatingGateway> ratingGateway_;
        std::shared_ptr<gateway::IMetadataGateway> metadataGateway_;
    };
}