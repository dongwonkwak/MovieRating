#pragma once

#include "common/expected.h"
#include "metadata/model/metadata.h"
#include "movie/gateway/gateway.h"
#include "movie/model/movie.h"

#include <string>
#include <memory>

namespace movie::controller
{
    class Controller
    {
    public:
        /**
         * @brief Construct a new Controller object
         * 
         * @param ratingGateway Rating서비스 게이트웨이
         * @param metadataGateway Movie metadata 게이트웨이
         */
        explicit Controller(
            const std::shared_ptr<gateway::IRatingGateway>& ratingGateway, 
            const std::shared_ptr<gateway::IMetadataGateway>& metadataGateway);
        /**
         * @brief Movie Detail값을 리턴한다. rating, metadata 서비스에서 값을 얻어 movie detail값을 얻는다.
         * 
         * @param id 
         * @return common::expected<movie::model::MovieDetails> 
         */
        auto Get(const std::string& id) -> common::expected<movie::model::MovieDetails>;
    private:
        std::shared_ptr<gateway::IRatingGateway> ratingGateway_;
        std::shared_ptr<gateway::IMetadataGateway> metadataGateway_;
    };
}