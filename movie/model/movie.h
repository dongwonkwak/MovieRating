#pragma once

#include "metadata/model/metadata.h"
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
    };
}

