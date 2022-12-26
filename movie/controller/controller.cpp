#include "spdlog/spdlog.h"
#include "movie/controller/controller.h"

namespace movie::controller
{
    Controller::Controller(
        std::unique_ptr<gateway::IRatingGateway> ratingGateway, 
        std::unique_ptr<gateway::IMetadataGateway> metadataGateway)
        : ratingGateway_(std::move(ratingGateway))
        , metadataGateway_(std::move(metadataGateway))
    {

    }


    auto Controller::Get(const std::string& id) 
        -> common::expected<movie::model::MovieDetails>
    {
        using namespace common;
        model::MovieDetails details;
        spdlog::debug("Controller got request - id: {}", id);

        return metadataGateway_->Get(id)
            .and_then([&](auto metadata) {
                details.metadata = metadata;
                return ratingGateway_->GetAggretatedRating(id, rating::model::RecordTypeMovie.data());
            })
            .map([&](auto rating) {
                details.rating = rating;
                return details;
            })
            .map_error([](auto e) { return unexpected{"can't find id"}; })
            .value();
    }
}