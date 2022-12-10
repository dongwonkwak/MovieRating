#pragma once

#include "metadata/model/metadata.h"
#include <cpprest/json.h>


namespace movie
{
    struct MovieDetails
    {
        double rating;
        metadata::Metadata metadata;

        web::json::value AsJSON() const
        {
            auto res = web::json::value::object();
            res["rating"] = web::json::value::number(rating);
            res["metadata"] = metadata.AsJSON();

            return res;
        }
    };
}

