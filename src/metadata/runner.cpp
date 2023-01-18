#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <boost/program_options.hpp>

#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/on_scope_exit.hpp>

#include "config/config.h"
#include "discovery/registry.h"
#include "metadata/runner.h"
#include "metadata/application.h"
#include "metadata/controller/controller.h"
#include "metadata/service/grpc/metadata_service.h"
#include "metadata/repository/repository.h"
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
            spdlog::error("requires config-file option. terminate program.");
            exit(-1);
        }

        Application app(vm);
        auto configService = app()->resolve<config::Config>();
        if (configService == nullptr)
        {
            spdlog::error("configService is null. terminate program.");
            exit(-1);
        }
        // get service name
        auto serviceName = configService->get<std::string>("application.name");
        // get service address(grpc)
        auto port = configService->get<ushort>("grpc.server.port");
        auto addr = fmt::format("{}:{}", 
            configService->get<std::string>("grpc.server.host"),
            port);
        // create controller
        auto controller = app()->resolve<controller::Controller>();
        // create consul registry
        auto registry = app()->resolve<discovery::Registry>();
        service::grpc::MetadataService server(controller, addr);

        cppcoro::static_thread_pool thread_pool;

        auto serviceId = discovery::GenerateServiceID(serviceName);
        registry->Register(serviceId, serviceName, std::to_string(port));

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