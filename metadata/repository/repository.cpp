#include "repository/repository.h"

namespace metadata::repository
{
    common::expected<model::Metadata> Repository::Get(std::string_view id)
    {
        if (data_.find(id.data()) == data_.end())
        {
            return common::unexpected{"can't find id"};
        }

        return data_[id.data()];
    }

    void Repository::Put(std::string_view id, const model::Metadata& data)
    {
        data_[id.data()] = data;
    }
}