#pragma once

#include "config/config.h"
#include "config/ServiceProvider.h"
#include "discovery/consul.h"
#include "movie/controller/controller.h"

#include <boost/program_options.hpp>

namespace movie
{
    class Application
    {
    public:
        Application(const boost::program_options::variables_map& vm);
    private:
        void CreateRegistry();
        void CreateController();
    private:
        boost::program_options::variables_map vm_;
        DECLARE_SHARED_SERVICE(std::shared_ptr<config::Config>, configService_);
        DECLARE_SHARED_SERVICE(std::shared_ptr<discovery::Registry>, registryService_);
        DECLARE_SHARED_SERVICE(std::shared_ptr<controller::Controller>, controller_);
    };
}