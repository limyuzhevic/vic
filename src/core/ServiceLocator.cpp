#include "ServiceLocator.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <typeinfo>

namespace nlm {

struct ServiceLocator::Impl {
    // Service management
    std::unordered_map<std::string, std::shared_ptr<IService>> servicesByName;
    std::unordered_map<std::type_index, std::shared_ptr<IService>> servicesByType;
    std::unordered_map<std::string, ServiceMetadata> serviceMetadata;
    std::unordered_map<std::string, ServiceState> serviceStates;
    std::unordered_map<std::string, std::map<std::string, std::any>> serviceConfigs;
    std::unordered_map<std::string, std::map<std::string, std::any>> serviceDependencies;
    std::unordered_map<std::string, std::vector<std::string>> serviceTags;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Statistics
    size_t registerCount = 0;
    size_t initializeCount = 0;
    size_t shutdownCount = 0;
    size_t errorCount = 0;
};

ServiceLocator::ServiceLocator() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("ServiceLocator initialized");
}

ServiceLocator::~ServiceLocator() {
    NLM_LOG_INFO("ServiceLocator shutdown");
    // Cleanup all services
    for (const auto& pair : pImpl->servicesByName) {
        if (pair.second->getState() == ServiceState::Ready) {
            pair.second->shutdown();
        }
    }
}

bool ServiceLocator::registerService(std::shared_ptr<IService> service) {
    if (!service) {
        NLM_LOG_ERROR("Cannot register null service");
        return false;
    }
    
    const auto& metadata = service->getMetadata();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if service with same name already exists
    if (pImpl->servicesByName.find(metadata.name) != pImpl->servicesByName.end()) {
        NLM_LOG_ERROR("Service already registered: " + metadata.name);
        return false;
    }
    
    pImpl->servicesByName[metadata.name] = service;
    pImpl->servicesByType[metadata.serviceType] = service;
    pImpl->serviceMetadata[metadata.name] = metadata;
    pImpl->serviceStates[metadata.name] = service->getState();
    
    // Extract tags from metadata
    if (metadata.metadata.find("tags") != metadata.metadata.end()) {
        auto tags = std::any_cast<std::vector<std::string>>(metadata.metadata.at("tags"));
        pImpl->serviceTags[metadata.name] = tags;
    }
    
    NLM_LOG_INFO("Service registered: " + metadata.name + " (" + std::string(metadata.serviceType.name()) + ")");
    pImpl->registerCount++;
    return true;
}

bool ServiceLocator::registerService(std::shared_ptr<ITypedService> service) {
    if (!service) {
        NLM_LOG_ERROR("Cannot register null typed service");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto implService = service->getServiceImpl();
    if (!implService) {
        NLM_LOG_ERROR("Typed service has null implementation");
        return false;
    }
    
    const auto& metadata = implService->getMetadata();
    
    // Check if service with same name already exists
    if (pImpl->servicesByName.find(service->getServiceName()) != pImpl->servicesByName.end()) {
        NLM_LOG_ERROR("Service already registered: " + service->getServiceName());
        return false;
    }
    
    pImpl->servicesByName[service->getServiceName()] = implService;
    pImpl->servicesByType[service->getServiceType()] = implService;
    pImpl->serviceMetadata[service->getServiceName()] = metadata;
    pImpl->serviceStates[service->getServiceName()] = implService->getState();
    
    NLM_LOG_INFO("Typed service registered: " + service->getServiceName() + 
                " (" + std::string(service->getServiceType().name()) + ")");
    pImpl->registerCount++;
    return true;
}

bool ServiceLocator::unregisterService(const std::string& serviceName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->servicesByName.find(serviceName);
    if (it == pImpl->servicesByName.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return false;
    }
    
    auto service = it->second;
    
    // Shutdown if initialized
    if (service->getState() == ServiceState::Ready) {
        service->shutdown();
    }
    
    // Remove from all maps
    pImpl->servicesByName.erase(it);
    
    auto typeIt = pImpl->servicesByType.begin();
    while (typeIt != pImpl->servicesByType.end()) {
        if (typeIt->second.get() == service.get()) {
            typeIt = pImpl->servicesByType.erase(typeIt);
        } else {
            ++typeIt;
        }
    }
    
    pImpl->serviceMetadata.erase(serviceName);
    pImpl->serviceStates.erase(serviceName);
    pImpl->serviceConfigs.erase(serviceName);
    pImpl->serviceDependencies.erase(serviceName);
    pImpl->serviceTags.erase(serviceName);
    
    NLM_LOG_INFO("Service unregistered: " + serviceName);
    return true;
}

bool ServiceLocator::unregisterService(std::type_index serviceType) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->servicesByType.find(serviceType);
    if (it == pImpl->servicesByType.end()) {
        NLM_LOG_ERROR("Service not found for type: " + std::string(serviceType.name()));
        return false;
    }
    
    auto service = it->second;
    
    // Shutdown if initialized
    if (service->getState() == ServiceState::Ready) {
        service->shutdown();
    }
    
    // Find and remove from name map
    auto nameIt = pImpl->servicesByName.begin();
    while (nameIt != pImpl->servicesByName.end()) {
        if (nameIt->second.get() == service.get()) {
            pImpl->serviceMetadata.erase(nameIt->first);
            pImpl->serviceStates.erase(nameIt->first);
            pImpl->serviceConfigs.erase(nameIt->first);
            pImpl->serviceDependencies.erase(nameIt->first);
            pImpl->serviceTags.erase(nameIt->first);
            nameIt = pImpl->servicesByName.erase(nameIt);
        } else {
            ++nameIt;
        }
    }
    
    pImpl->servicesByType.erase(it);
    
    NLM_LOG_INFO("Service unregistered for type: " + std::string(serviceType.name()));
    return true;
}

std::shared_ptr<IService> ServiceLocator::getService(const std::string& serviceName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->servicesByName.find(serviceName);
    if (it == pImpl->servicesByName.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return nullptr;
    }
    
    return it->second;
}

std::shared_ptr<IService> ServiceLocator::getService(std::type_index serviceType) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->servicesByType.find(serviceType);
    if (it == pImpl->servicesByType.end()) {
        NLM_LOG_ERROR("Service not found for type: " + std::string(serviceType.name()));
        return nullptr;
    }
    
    return it->second;
}

std::vector<std::string> ServiceLocator::getServiceNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->servicesByName.size());
    for (const auto& pair : pImpl->servicesByName) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::type_index> ServiceLocator::getServiceTypes() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::type_index> types;
    types.reserve(pImpl->servicesByType.size());
    for (const auto& pair : pImpl->servicesByType) {
        types.push_back(pair.first);
    }
    return types;
}

ServiceMetadata ServiceLocator::getServiceMetadata(const std::string& serviceName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->serviceMetadata.find(serviceName);
    if (it == pImpl->serviceMetadata.end()) {
        NLM_LOG_ERROR("Service metadata not found: " + serviceName);
        return ServiceMetadata();
    }
    
    return it->second;
}

ServiceState ServiceLocator::getServiceState(const std::string& serviceName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->serviceStates.find(serviceName);
    if (it == pImpl->serviceStates.end()) {
        return ServiceState::Destroyed;
    }
    
    return it->second;
}

bool ServiceLocator::initializeService(const std::string& serviceName,
                                       const std::map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->servicesByName.find(serviceName);
    if (it == pImpl->servicesByName.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return false;
    }
    
    auto service = it->second;
    
    if (service->getState() == ServiceState::Ready) {
        NLM_LOG_INFO("Service already initialized: " + serviceName);
        return true;
    }
    
    NLM_LOG_INFO("Initializing service: " + serviceName);
    
    try {
        // Configure if config provided
        if (!config.empty()) {
            configureService(serviceName, config);
        }
        
        if (!service->initialize(config)) {
            NLM_LOG_ERROR("Failed to initialize service: " + serviceName);
            return false;
        }
        
        pImpl->serviceStates[serviceName] = service->getState();
        pImpl->initializeCount++;
        
        NLM_LOG_INFO("Service initialized successfully: " + serviceName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing service " + serviceName + ": " + std::string(e.what()));
        pImpl->errorCount++;
        return false;
    }
}

bool ServiceLocator::shutdownService(const std::string& serviceName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->servicesByName.find(serviceName);
    if (it == pImpl->servicesByName.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return false;
    }
    
    auto service = it->second;
    
    if (service->getState() != ServiceState::Ready) {
        NLM_LOG_INFO("Service not initialized: " + serviceName);
        return true;
    }
    
    NLM_LOG_INFO("Shutting down service: " + serviceName);
    
    try {
        service->shutdown();
        pImpl->serviceStates[serviceName] = ServiceState::ShuttingDown;
        pImpl->shutdownCount++;
        
        NLM_LOG_INFO("Service shutdown successfully: " + serviceName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down service " + serviceName + ": " + std::string(e.what()));
        return false;
    }
}

bool ServiceLocator::resetService(const std::string& serviceName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->servicesByName.find(serviceName);
    if (it == pImpl->servicesByName.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return false;
    }
    
    auto service = it->second;
    
    NLM_LOG_INFO("Resetting service: " + serviceName);
    
    try {
        service->shutdown();
        pImpl->serviceStates[serviceName] = ServiceState::Created;
        
        // Clear configuration
        pImpl->serviceConfigs.erase(serviceName);
        pImpl->serviceDependencies.erase(serviceName);
        
        NLM_LOG_INFO("Service reset successfully: " + serviceName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception resetting service " + serviceName + ": " + std::string(e.what()));
        return false;
    }
}

bool ServiceLocator::configureService(const std::string& serviceName,
                                     const std::map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->servicesByName.find(serviceName);
    if (it == pImpl->servicesByName.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return false;
    }
    
    pImpl->serviceConfigs[serviceName] = config;
    NLM_LOG_INFO("Service configuration updated: " + serviceName);
    return true;
}

std::map<std::string, std::any> ServiceLocator::getServiceConfig(const std::string& serviceName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->serviceConfigs.find(serviceName);
    if (it == pImpl->serviceConfigs.end()) {
        return std::map<std::string, std::any>();
    }
    
    return it->second;
}

bool ServiceLocator::injectDependencies(const std::string& serviceName,
                                        const std::map<std::string, std::any>& dependencies) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->servicesByName.find(serviceName);
    if (it == pImpl->servicesByName.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return false;
    }
    
    pImpl->serviceDependencies[serviceName] = dependencies;
    NLM_LOG_INFO("Service dependencies injected: " + serviceName);
    return true;
}

std::map<std::string, std::any> ServiceLocator::getServiceDependencies(const std::string& serviceName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->serviceDependencies.find(serviceName);
    if (it == pImpl->serviceDependencies.end()) {
        return std::map<std::string, std::any>();
    }
    
    return it->second;
}

std::vector<std::string> ServiceLocator::getReadyServices() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> readyServices;
    
    for (const auto& pair : pImpl->serviceStates) {
        if (pair.second == ServiceState::Ready) {
            readyServices.push_back(pair.first);
        }
    }
    
    return readyServices;
}

std::vector<std::string> ServiceLocator::getServicesByTag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> taggedServices;
    
    for (const auto& pair : pImpl->serviceTags) {
        const auto& tags = pair.second;
        if (std::find(tags.begin(), tags.end(), tag) != tags.end()) {
            taggedServices.push_back(pair.first);
        }
    }
    
    return taggedServices;
}

bool ServiceLocator::hasService(const std::string& serviceName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->servicesByName.find(serviceName) != pImpl->servicesByName.end();
}

bool ServiceLocator::hasService(std::type_index serviceType) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->servicesByType.find(serviceType) != pImpl->servicesByType.end();
}

ServiceLocator& ServiceLocator::getInstance() {
    static ServiceLocator instance;
    return instance;
}

void ServiceLocator::reset() {
    static ServiceLocator instance;
    instance.~ServiceLocator();
    new (&instance) ServiceLocator();
}
} // namespace nlm