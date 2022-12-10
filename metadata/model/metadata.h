#pragma once

#include <cpprest/json.h>

#include <string>

namespace metadata
{

struct Metadata
{
    std::string id;
    std::string title;
    std::string description;
    std::string director;

    web::json::value AsJSON() const
    {
        auto res = web::json::value::object();
        res["id"] = web::json::value::string(id);
        res["title"] = web::json::value::string(title);
        res["description"] = web::json::value::string(description);
        res["director"] = web::json::value::string(director);

        return res;
    }

    static Metadata FromJSON(web::json::value object)
    {
        Metadata result;
        result.id = object["id"].as_string();
        result.title = object["title"].as_string();
        result.description = object["description"].as_string();
        result.director = object["director"].as_string();
        return result;
    }
};


}
