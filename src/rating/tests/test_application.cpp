#include "test_application.h"
#include "rating/ingester/kafka/ingester.h"
using namespace rating::ingester::kafka;

#include "rating/model/rating.h"
using namespace rating::model;

#include "discovery/consul.h"
#include "rating/repository/postgresql/postgresql.h"
#include "rating/controller/controller.h"

std::shared_ptr<Hypodermic::Container> TestApplication::operator()()
{
    return container_;
}


TestApplication::TestApplication()
{
    auto consul = discovery::ConsulRegistry::Create(RATING_CONSUL_ENDPOINT);
    auto serviceName = "rating";
    auto serviceId = discovery::GenerateServiceID(serviceName);
    
    builder_.registerInstance(consul);
    std::string host = RATING_GRPC_ENDPOINT;
    std::string port = host.substr(host.find(':') + 1, host.size());
    consul->Register(serviceId, serviceName, port);
        
    builder_.registerInstanceFactory([](Hypodermic::ComponentContext& context) {
        auto repository = std::make_shared<rating::repository::postresql::Repository>(RATING_POSTGRESQL_CONNECTION_URL);
        return std::make_shared<rating::controller::Controller>(repository, nullptr);
    });

    container_ = builder_.build();
}

