#pragma once

#include "metadata/repository/repository.h"
#include <pqxx/pqxx>

#include <memory>

namespace config
{
    class Config;
}

namespace metadata::repository::postgresql
{
    class Repository : public IRepository
    {
    public:
        Repository(const std::shared_ptr<config::Config>& config);

        auto Get(std::string_view id) -> common::expected<model::Metadata> override;
        void Put(std::string_view id, const model::Metadata& data) override;
    private:
        std::unique_ptr<::pqxx::connection> conn_;
    };
}