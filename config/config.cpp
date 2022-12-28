#include "config/config.h"
#include <yaml-cpp/yaml.h>
#include <range/v3/all.hpp>

#include <iostream>

static std::unique_ptr<YAML::Node> rootNode_;

template <ranges::input_iterator T>
YAML::Node GetNode(const YAML::Node& node, T first, T last)
{
    YAML::Node currentNode = YAML::Clone(node);
    for (auto iter = first; iter != last; ++iter)
    {
        currentNode = currentNode[*iter];
    }

    return currentNode;
}

namespace config
{
    std::shared_ptr<config::Config> Config::Create(const std::string& filename)
    {
        return std::shared_ptr<config::Config>(new config::Config(filename));
    }

    Config::Config()
    {

    }

    Config::Config(const std::string& filename)
    {
        if (!filename.empty())
        {
            Load(filename);
        }
    }

    void Config::Load(const std::string& filename)
    {
        try
        {
            rootNode_ = std::make_unique<YAML::Node>(YAML::LoadFile(filename));
        }
        catch (const YAML::Exception& e)
        {
            throw std::runtime_error(e.what());
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(e.what());
        }
    }

    template <typename T>
    T Config::get(const std::string& k)
    {
        using namespace ranges::views;
        auto inp = k 
            | split('.')
            | transform([](auto&& rng) {
                return std::string(&*rng.begin(), ranges::distance(rng));
            });
        
        return GetNode(*rootNode_, inp.begin(), inp.end()).template as<T>();
    }

    template std::string Config::get<std::string>(const std::string&);
    template ushort Config::get<ushort>(const std::string&);
    template int Config::get<int>(const std::string&);
}