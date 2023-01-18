#pragma once

#include "movie/gateway/gateway.h"
#include "discovery/registry.h"

#include <memory>

namespace movie::gateway::metadata::grpc
{
    class MetadataGateway : public IMetadataGateway
    {
    public:
        /**
         * @brief Construct a new Metadata Gateway object
         * 
         * @param registry 서비스 레지스트리
         */
        MetadataGateway(const std::shared_ptr<discovery::Registry>& registry);
        ~MetadataGateway();
        /**
         * @brief id에 해당하는 Movie Metadata정보를 리턴힌다.
         *  Movie 서비스를 호출하여 해당값을 얻는다.
         * 
         * @param id 
         * @return common::expected<::metadata::model::Metadata> 
         */
        auto Get(const std::string& id) 
            -> common::expected<::metadata::model::Metadata> override;        
    private:
        std::shared_ptr<discovery::Registry> registry_;
    };
}