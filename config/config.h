#pragma once

#include <memory>
#include <string>


namespace config
{
    class Config
    {
    public:
        static std::shared_ptr<config::Config> Create(const std::string& filename = std::string());

        void Load(const std::string& filename);

        template <typename T>
        T get(const std::string& k);
    private:
        Config();
        Config(const std::string& filename);        
    };
}

