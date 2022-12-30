#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/on_scope_exit.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/schedule_on.hpp>
#include <boost/program_options.hpp>

#include "config/service_provider.h"
#include "config/config.h"
#include "rating/runner.h"
#include "rating/application.h"
#include "rating/service/grpc/rating_service.h"
#include "discovery/consul.h"



namespace rating
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
            std::cout << "configService is null. terminate program.";
            exit(-1);
        }
        auto addr = fmt::format("{}:{}", 
            configService->get<std::string>("grpc.server.host"),
            configService->get<ushort>("grpc.server.port"));
        spdlog::info(fmt::format("Rating Service Listening for requests at: {}", addr));
        GET_SERVICE(std::shared_ptr<controller::Controller>, controller);
        rating::service::grpc::RatingService server(controller, addr);
        GET_SERVICE(std::shared_ptr<discovery::Registry>, registry);
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