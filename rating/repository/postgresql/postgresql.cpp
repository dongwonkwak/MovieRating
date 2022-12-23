#include <fmt/core.h>
#include "rating/repository/postgresql/postgresql.h"

namespace rating::repository::postresql
{
    Repository::Repository(const std::string& connStr)
        : conn_(new ::pqxx::connection(connStr))
    {

    }

    auto Repository::Get(const RecordID& recordID, const RecordType& recordType)
        -> common::expected<std::vector<common::expected<model::Rating>>>
    {
        pqxx::work tx{*conn_};
        std::vector<common::expected<model::Rating>> res;

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
                 
                res.emplace_back(rating);
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
            pqxx::result r{tx.exec(
                fmt::format("INSERT INTO  ratings (record_id, record_type, user_id, value) VALUES ('{}','{}', '{}', {})", 
                    recordId, recordType, rating.userId, rating.ratingValue)
            )};
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