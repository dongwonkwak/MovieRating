#pragma once


#include <boost/program_options.hpp>
#include "Hypodermic/Hypodermic.h"

namespace rating
{
    class Application
    {
    public:
        Application(const boost::program_options::variables_map& vm);

        std::shared_ptr<Hypodermic::Container> operator()();
    private:
        void CreateRegistry();
        void CreateController();

        Hypodermic::ContainerBuilder builder_;
        std::shared_ptr<Hypodermic::Container> container_;
    };
}