#include "movie/controller/controller.h"

namespace movie
{
    Controller::Controller(
        std::unique_ptr<IRatingGateway>&& ratingGateway, 
        std::unique_ptr<IMetadataGateway>&& metadataGateway)
        : ratingGateway_(std::move(ratingGateway))
        , metadataGateway_(std::move(metadataGateway))
    {

    }

    auto Controller::Get(const std::string& id) -> common::expected<MovieDetails>
    {
        auto metadata = metadataGateway_->Get(id);
        if (!metadata.has_value())
        {
            return common::unexpected{"can't find id"};
        }
        //auto details

        auto rating = ratingGateway_->GetAggretatedRating(id, rating::RecordTypeMovie.data());
        if (!rating.has_value())
        {
            return common::unexpected{"can't find rating"};
        }
        MovieDetails details;
        details.metadata = metadata.value();
        details.rating = rating.value();

        return details;
    }
}