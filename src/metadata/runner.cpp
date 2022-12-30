#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <boost/program_options.hpp>

#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/on_scope_exit.hpp>

#include "config/config.h"
#include "config/service_provider.h"
#include "discovery/registry.h"
#include "metadata/runner.h"
#include "metadata/application.h"
#include "metadata/controller/controller.h"
#include "metadata/service/grpc/metadata_service.h"

#include <iostream>

namespace metadata
{
    void Runner::run(int argc, char* argv[])
    {
        using namespace boost::program_options;
        options_description desc("Allowed options");
        desc.add_options()
        ("help,h", "produce a help screen")
        ("config-file,c", value<std::string>(),"path to config file");
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);

        if (!vm.count("config-file"))
        {
            std::cerr << "requires config-file option. terminate program.\n";
            exit(-1);
        }

        Application app(vm);
        GET_SERVICE(std::shared_ptr<config::Config>, configService);
        if (configService == nullptr)
        {
            std::cerr << "configService is null. terminate program.\n";
            exit(-1);
        }
        auto addr = fmt::format("{}:{}", 
            configService->get<std::string>("grpc.server.host"),
            configService->get<ushort>("grpc.server.port"));
        GET_SERVICE(std::shared_ptr<controller::Controller>, controller);
        GET_SERVICE(std::shared_ptr<discovery::Registry>, registry);
        service::grpc::MetadataService server(controller, addr);

        cppcoro::static_thread_pool thread_pool;

        auto serviceId = registry->GetServiceID();

        auto health_check = [&]() -> cppcoro::task<>
        {
            while (true)
            {
                co_await thread_pool.schedule();

                registry->ReportHealthyState(serviceId);
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
            co_return;
        };

        cppcoro::sync_wait(cppcoro::when_all(
            [&]() -> cppcoro::task<>
            {
                co_await health_check();
            }(),
            [&]() -> cppcoro::task<>
            {
                server.start();
                co_return;
            }()
        ));
    }
}