#pragma once

#include "Hypodermic/Hypodermic.h"


class TestApplication
{
public:
    TestApplication();
     std::shared_ptr<Hypodermic::Container> operator()();
private:
    Hypodermic::ContainerBuilder builder_;
    std::shared_ptr<Hypodermic::Container> container_;
};