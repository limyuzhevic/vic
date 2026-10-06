#pragma once

#include "ServiceRegistry.hpp"
#include "../core/Logger/Logger.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace nlm {

class ServiceRegistry::Impl {
public:
    struct ServiceEntry {
        std::unique_ptr<class Service> service;
        std::string pluginName;
        ServiceInfo info;
    };
    
    ServiceRegistry() : initialized_(false) {
        initialize();
    }
    
    ~ServiceRegistry() {
        clear();
    }
    
    void initialize() {
        if (initialized_) {
            return;
        }
        initialized_ = true;
    }
    
    bool registerService(const std::string& serviceName, 
                        std::unique_ptr<class Service> service,
                        const std::string& pluginName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (serviceRegistry_.count(serviceName) > 0) {
            NLM_LOG_ERROR(std::string("Service already registered: ") + serviceName);
            return false;
        }
        
        ServiceEntry entry;
        entry.service = std::move(service);
        entry.pluginName = pluginName;
        entry.info = entry.service->getInfo();
        
        serviceRegistry_[serviceName] = std::move(entry);
        
        NLM_LOG_INFO(std::string("Service registered: ") + serviceName + " by plugin " + pluginName);
        return true;
    }
    
    std::unique_ptr<class Service> getService(const std::string& serviceName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (serviceRegistry_.count(serviceName) == 0) {
            return nullptr;
        }
        
        // Return service (caller takes ownership)
        auto service = std::move(serviceRegistry_[serviceName].service);
        serviceRegistry_.erase(serviceName);
        return service;
    }
    
    bool hasService(const std::string& serviceName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return serviceRegistry_.count(serviceName) > 0;
    }
    
    void unregisterServicesByPlugin(const std::string& pluginName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> servicesToRemove;
        for (const auto& pair : serviceRegistry_) {
            if (pair.second.pluginName == pluginName) {
                servicesToRemove.push_back(pair.first);
            }
        }
        
        for (const auto& serviceName : servicesToRemove) {
            serviceRegistry_.erase(serviceName);
        }
        
        if (!servicesToRemove.empty()) {
            NLM_LOG_INFO(std::string("Unregistered ") + std::to_string(servicesToRemove.size()) + 
                        " services by plugin: " + pluginName);
        }
    }
    
    void unregisterService(const std::string& serviceName) {
        std::lock_guard<std::mutex> lock(mutex_);
        serviceRegistry_.erase(serviceName);
    }
    
    std::vector<ServiceInfo> getAllServices() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ServiceInfo> services;
        services.reserve(serviceRegistry_.size());
        for (const auto& pair : serviceRegistry_) {
            services.push_back(pair.second.info);
        }
        return services;
    }
    
    std::vector<std::string> getPluginNamesForService(const std::string& serviceName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> pluginNames;
        
        if (serviceRegistry_.count(serviceName) == 0) {
            return pluginNames;
        }
        
        pluginNames.push_back(serviceRegistry_.at(serviceName).pluginName);
        return pluginNames;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        serviceRegistry_.clear();
    }
    
private:
    bool initialized_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, ServiceEntry> serviceRegistry_;
};

} // namespace nlm
