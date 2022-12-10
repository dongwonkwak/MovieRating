#include "rating/model/rating.h"

namespace rating
{
    web::json::value AsJSON(const common::expected<std::vector<common::expected<Rating>>>& rating)
    {
        auto res = web::json::value::array(rating->size());
        int index = 0;
        for (const auto r : rating.value())
        {
            res[index++] = r->AsJSON();
        }
        return res;
    }
}