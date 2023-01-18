#include <spdlog/spdlog.h>

#include "metadata/repository/postgresql/postgresql.h"
#include "config/config.h"

namespace metadata::repository::postgresql
{
    Repository::Repository(const std::shared_ptr<config::Config>& config)
    {
        auto connectString = 
                fmt::format("postgresql://{}:{}@{}",
                    config->get<std::string>("application.datasource.user"),
                    config->get<std::string>("application.datasource.password"),
                    config->get<std::string>("application.datasource.url"));
        conn_ = std::make_unique<::pqxx::connection>(connectString);
    }

    auto Repository::Get(std::string_view id) -> common::expected<model::Metadata>
    {
        pqxx::work tx{*conn_};
        model::Metadata res;

        try
        {
            pqxx::result r{tx.exec(
                fmt::format("SELECT title, description, director FROM movies WHERE id='{}'", id.data())
            )};
            if (r.size() == 0)
            {
                return common::unexpected{"there is no record"};
            }
            const auto& selected = r[0];
            res.title = selected[0].c_str();
            res.description = selected[1].c_str();
            res.director = selected[2].c_str();
        }
        catch (const pqxx::sql_error& e)
        {
            return common::unexpected{fmt::format("SQL error: {}, Query: {}", e.what(), e.query())};
        }
        catch (const std::exception& e)
        {
            return common::unexpected{fmt::format("Error: {}", e.what())};
        }

        return res;
    }

    void Repository::Put(std::string_view id, const model::Metadata& data)
    {
        pqxx::work tx{*conn_};
        try
        {
            auto query = fmt::format("INSERT INTO movies (id, title, description, director) VALUES ('{}', '{}', '{}', '{})",
                id, data.title, data.description, data.director
            );
            spdlog::info("[Repository::Put] {}", query);
            tx.exec(query);
            tx.commit();
        }
        catch (const pqxx::sql_error& e)
        {
            spdlog::error("SQL error: {}, Query:{}", e.what(), e.query());
        }
        catch (const std::exception& e)
        {
            spdlog::error("Error: {}", e.what());
        }
    }
}