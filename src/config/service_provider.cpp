#include "config/service_provider.h"


#include <mutex>
#include <iostream>

namespace config
{

static std::unordered_map<std::string, std::unordered_map<std::string, void*>> services;
static std::mutex servicesMutex;

void ServiceProvider::registerService(const std::string& typeName, const std::string& serviceName, void* service)
{
    std::cout << "registerService: " << typeName << ", serviceName: " << serviceName << std::endl;
    std::lock_guard lock(servicesMutex);
    auto& bucket = services[typeName];
    auto iter = bucket.find(serviceName);
    if (iter != bucket.end())
    {
        throw std::runtime_error("[registerService]: Error. Service already exists");
    }
    bucket[serviceName] = service;
}

void ServiceProvider::unregisterService(const std::string& typeName, const std::string& serviceName)
{
    std::cout << "unregister: " << typeName << ", serviceName: " << serviceName << std::endl;
    std::lock_guard lock(servicesMutex);
    auto iter = services.find(typeName);
    if (iter == services.end() || iter->second.size() == 0)
    {
        throw std::runtime_error("[unregisterService]: Error. Can't find service type.");
    }
    auto& bucket = iter->second;
    auto service_iter = bucket.find(serviceName);
    if (service_iter == bucket.end())
    {
        throw std::runtime_error("[unregisterService]: Error. Can't find service name.");
    }
    bucket.erase(service_iter);
    if (bucket.size() == 0)
    {
        services.erase(iter);
    }
}

void* ServiceProvider::getService(const std::string& typeName, const std::string& serviceName)
{
    std::lock_guard lock(servicesMutex);
    auto bucketIt = services.find(typeName);
    if (bucketIt == services.end() || bucketIt->second.size() == 0)
    {
        throw std::runtime_error("[getService]: Error. Can't find service type");
    }
    auto& bucket = bucketIt->second;
    auto serviceIt = bucket.find(serviceName);
    if (serviceIt == bucket.end())
    {
        throw std::runtime_error("[getService]: Error. Can't find service name");
    }
    return serviceIt->second;
}

void* ServiceProvider::getService(const std::string& typeName)
{
    std::lock_guard lock(servicesMutex);
    auto bucketIt = services.find(typeName);
    if (bucketIt == services.end() || bucketIt->second.size() == 0)
    {
        throw std::runtime_error("[getService]: Error. Can't find service type");
    }
    auto bucket = bucketIt->second;
    if (bucket.size() > 1)
    {
        throw std::runtime_error("");
    }
    return bucket.begin()->second;
}

void ServiceProvider::destroy()
{
    /*if (components.size() > 0)
    {

    }*/
}

}