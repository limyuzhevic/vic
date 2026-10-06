#pragma once

#include "../core/Logger/Logger.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace nlm {

class ServiceRegistry {
public:
    ServiceRegistry();
    ~ServiceRegistry();
    
    bool registerService(const std::string& serviceName, 
                        std::unique_ptr<class Service> service,
                        const std::string& pluginName);
    
    std::unique_ptr<class Service> getService(const std::string& serviceName);
    bool hasService(const std::string& serviceName) const;
    
    void unregisterServicesByPlugin(const std::string& pluginName);
    void unregisterService(const std::string& serviceName);
    
    std::vector<ServiceInfo> getAllServices() const;
    std::vector<std::string> getPluginNamesForService(const std::string& serviceName) const;
    
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
