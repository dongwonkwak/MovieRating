#pragma once


#include <boost/program_options.hpp>
#include "Hypodermic/Hypodermic.h"

namespace movie
{
    class Application
    {
    public:
        Application(const boost::program_options::variables_map& vm);

        std::shared_ptr<Hypodermic::Container> operator()();
    private:
        void RegisterRegistry();
        void RegisterController();
        
        Hypodermic::ContainerBuilder builder_;
        std::shared_ptr<Hypodermic::Container> container_;
    };
}