#pragma once

#include <vector>
#include <unordered_map>
#include <string>

namespace config
{

class ServiceProvider
{
private:
    static void registerService(const std::string& typeName, const std::string& serviceName, void* service);
    static void unregisterService(const std::string& typeName, const std::string& serviceName);
public:
    template <typename T>
    class Service
    {
        std::string type_;
        std::string name_;
        T object_;
    public:
        Service(const std::string& name, const T& object)
            : type_(typeid(T).name())
            , name_(name)
            , object_(object)
        {
            ServiceProvider::registerService(type_, name_, &object_);
        }
        Service(const T& object)
            : Service("NoName", object)
        {}
        ~Service()
        {
            ServiceProvider::unregisterService(type_, name_);
        }

        T getObject()
        {
            return object_;
        }
    };
    static void destroy();
    static void* getService(const std::string& typeName);
    static void* getService(const std::string& typeName, const std::string& serviceName);
};

}

#define CREATE_SERVICE(TYPE, NAME) \
config::ServiceProvider::Service<TYPE> NAME = config::ServiceProvider::Service<TYPE>

#define GET_SERVICE(TYPE, NAME) \
TYPE& NAME = (*((TYPE*)config::ServiceProvider::getService(typeid(TYPE).name())))


#define DECLARE_SHARED_SERVICE(TYPE, NAME) std::shared_ptr<config::ServiceProvider::Service<TYPE>> NAME
#define CREATE_SHARED_SERVICE(TYPE, NAME) \
DECLARE_SHARED_SERVICE(TYPE, NAME) = std::make_shared<config::ServiceProvider::Service<TYPE>>