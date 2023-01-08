#include <spdlog/spdlog.h>
#include <fmt/core.h>

#include "rating/repository/postgresql/postgresql.h"
#include "config/config.h"

namespace rating::repository::postresql
{
    Repository::Repository(const std::string& connStr)
        : conn_(new ::pqxx::connection(connStr))
    {

    }

    Repository::Repository(std::string_view host, unsigned short port, std::string_view dbname, std::string_view user, std::string_view password)
    {
        conn_ = std::make_unique<::pqxx::connection>(fmt::format("host={} port={} dbname={} user={} password={}", host, port, dbname, user, password));
    }

    Repository::Repository(const std::shared_ptr<config::Config>& config)
    {
        auto connectString = 
                fmt::format("postgresql://{}:{}@{}",
                    config->get<std::string>("application.datasource.user"),
                    config->get<std::string>("application.datasource.password"),
                    config->get<std::string>("application.datasource.url"));
        conn_ = std::make_unique<::pqxx::connection>(connectString);
    }

    auto Repository::Get(const RecordID& recordID, const RecordType& recordType)
        -> common::expected<RatingSet>
    {
        pqxx::work tx{*conn_};
        RatingSet res;

        try
        {
            pqxx::result r{tx.exec(
                fmt::format(
                    "SELECT user_id, value FROM ratings WHERE record_id='{}' AND record_type='{}'", recordID, recordType)
            )};

            for (auto row : r)
            {
                model::Rating rating;
                rating.userId = row["user_id"].c_str();
                rating.ratingValue = row[1].as<int>();
                rating.recordId = recordID;
                rating.recordType = recordType;
                res.insert(rating);
                //res.emplace_back(rating);
            }

            tx.commit();
            return res;
        }
        catch (const pqxx::sql_error& e)
        {
            return common::unexpected{fmt::format("SQL error: {}, Query: {}", e.what(), e.query())};
        }
        catch (const std::exception& e)
        {
            return common::unexpected{fmt::format("Error: {}", e.what())};
        }
    }

    void Repository::Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating)
    {
        pqxx::work tx{*conn_};
        try
        {
            auto query = fmt::format("INSERT INTO ratings (record_id, record_type, user_id, value) VALUES ('{}', '{}', '{}', {})",
                recordId, recordType, rating.userId, rating.ratingValue);
            spdlog::info("[Repository::Put] {}", query);

            tx.exec(query);
            tx.commit();
        }
        catch (const pqxx::sql_error& e)
        {
            std::cerr << e.query() << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}