#include "test_application.h"

#include "discovery/consul.h"
#include "metadata/repository/repository.h"
#include "metadata/controller/controller.h"

std::shared_ptr<Hypodermic::Container> TestApplication::operator()()
{
    return container_;
}

TestApplication::TestApplication()
{
    auto consul = discovery::ConsulRegistry::Create(MOVIE_RATING_CONSUL_ENDPOINT);
    auto serviceName = "metadata";
    auto serviceId = discovery::GenerateServiceID(serviceName);

    builder_.registerInstance(consul);
    std::string host = METADATA_GRPC_ENDPOINT;
    std::string port = host.substr(host.find(':') + 1, host.size());
    consul->Register(serviceId, serviceName, port);

    builder_.registerInstanceFactory([](Hypodermic::ComponentContext& context) {
        auto repository = std::make_shared<metadata::repository::Repository>();
        return std::make_shared<metadata::controller::Controller>(repository);
    });

    container_ = builder_.build();
}

