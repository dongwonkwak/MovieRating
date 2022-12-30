#pragma once

#include "metadata/model/metadata.h"
#include "movie.pb.h"

#include <cpprest/json.h>


namespace movie::model
{
    struct MovieDetails
    {
        double rating;
        metadata::model::Metadata metadata;

        web::json::value AsJSON() const
        {
            auto res = web::json::value::object();
            res["rating"] = web::json::value::number(rating);
            res["metadata"] = metadata.AsJSON();

            return res;
        }

        auto MovieDetailsToProto() -> movie::MovieDetails
        {
            movie::MovieDetails result;
            result.set_rating(rating);
            result.mutable_metadata()->CopyFrom(metadata.MetadataToProto());

            return result;
        }

        static auto MovieDetailsFromProto(const movie::MovieDetails& data)
            -> movie::model::MovieDetails
        {
            movie::model::MovieDetails result;
            result.rating = data.rating();
            result.metadata = ::metadata::model::Metadata::MetadataFromProto(data.metadata());

            return result;
        }
    };
}

