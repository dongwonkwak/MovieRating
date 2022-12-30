#pragma once

#include "config/config.h"
#include "config/service_provider.h"
#include "discovery/consul.h"
#include "rating/controller/controller.h"

#include <boost/program_options.hpp>

namespace rating
{
    class Application
    {
    public:
        Application(const boost::program_options::variables_map& vm);
    private:
        void InitializeLogger();
        void CreateRegistry();
        void CreateController();
    private:
        boost::program_options::variables_map vm_;
        DECLARE_SHARED_SERVICE(std::shared_ptr<config::Config>, configService_);
        DECLARE_SHARED_SERVICE(std::shared_ptr<discovery::Registry>, registryService_);
        DECLARE_SHARED_SERVICE(std::shared_ptr<controller::Controller>, controller_);
    };
}