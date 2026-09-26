#include "FactoryPattern.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct FactoryRegistry::Impl {
    // Factory management
    std::unordered_map<std::string, std::shared_ptr<IFactory>> factories;
    std::unordered_map<std::string, FactoryMetadata> factoryMetadata;
    std::unordered_map<std::string, bool> factoryInitialized;
    
    // Factory methods registry
    std::unordered_map<std::string, std::map<std::string, std::function<std::any(const std::map<std::string, std::any>&)>>> factoryMethods;
    
    // Object pools
    std::unordered_map<std::string, std::shared_ptr<IObjectPool>> pools;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Statistics
    size_t registerCount = 0;
    size_t initializeCount = 0;
    size_t createCount = 0;
    size_t errorCount = 0;
};

FactoryRegistry::FactoryRegistry() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("FactoryRegistry initialized");
}

FactoryRegistry::~FactoryRegistry() {
    NLM_LOG_INFO("FactoryRegistry shutdown");
}

bool FactoryRegistry::registerFactory(std::shared_ptr<IFactory> factory) {
    if (!factory) {
        NLM_LOG_ERROR("Cannot register null factory");
        return false;
    }
    
    const auto& metadata = factory->getMetadata();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if factory with same name already exists
    if (pImpl->factories.find(metadata.name) != pImpl->factories.end()) {
        NLM_LOG_ERROR("Factory already registered: " + metadata.name);
        return false;
    }
    
    pImpl->factories[metadata.name] = factory;
    pImpl->factoryMetadata[metadata.name] = metadata;
    pImpl->factoryInitialized[metadata.name] = false;
    
    NLM_LOG_INFO("Factory registered: " + metadata.name + " (" + std::to_string(static_cast<int>(metadata.type)) + ")");
    pImpl->registerCount++;
    return true;
}

bool FactoryRegistry::unregisterFactory(const std::string& factoryName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(factoryName);
    if (it == pImpl->factories.end()) {
        NLM_LOG_ERROR("Factory not found: " + factoryName);
        return false;
    }
    
    // If factory is initialized, shutdown it first
    if (pImpl->factoryInitialized[factoryName]) {
        shutdownFactory(factoryName);
    }
    
    pImpl->factories.erase(it);
    pImpl->factoryMetadata.erase(factoryName);
    pImpl->factoryInitialized.erase(factoryName);
    
    // Clean up methods
    pImpl->factoryMethods.erase(factoryName);
    
    NLM_LOG_INFO("Factory unregistered: " + factoryName);
    return true;
}

bool FactoryRegistry::initializeFactory(const std::string& factoryName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(factoryName);
    if (it == pImpl->factories.end()) {
        NLM_LOG_ERROR("Factory not found: " + factoryName);
        return false;
    }
    
    auto factory = it->second;
    if (pImpl->factoryInitialized[factoryName]) {
        NLM_LOG_INFO("Factory already initialized: " + factoryName);
        return true;
    }
    
    NLM_LOG_INFO("Initializing factory: " + factoryName);
    
    try {
        if (!factory->initialize()) {
            NLM_LOG_ERROR("Failed to initialize factory: " + factoryName);
            return false;
        }
        
        pImpl->factoryInitialized[factoryName] = true;
        pImpl->initializeCount++;
        
        NLM_LOG_INFO("Factory initialized successfully: " + factoryName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing factory " + factoryName + ": " + std::string(e.what()));
        return false;
    }
}

bool FactoryRegistry::shutdownFactory(const std::string& factoryName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(factoryName);
    if (it == pImpl->factories.end()) {
        NLM_LOG_ERROR("Factory not found: " + factoryName);
        return false;
    }
    
    auto factory = it->second;
    
    NLM_LOG_INFO("Shutting down factory: " + factoryName);
    
    try {
        factory->shutdown();
        pImpl->factoryInitialized[factoryName] = false;
        
        NLM_LOG_INFO("Factory shutdown successfully: " + factoryName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down factory " + factoryName + ": " + std::string(e.what()));
        return false;
    }
}

std::any FactoryRegistry::createObject(const std::string& factoryName, const std::string& objectType,
                                       const std::map<std::string, std::any>& params) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if factory is initialized
    auto initIt = pImpl->factoryInitialized.find(factoryName);
    if (initIt == pImpl->factoryInitialized.end() || !initIt->second) {
        // Try to initialize automatically
        if (!initializeFactory(factoryName)) {
            NLM_LOG_ERROR("Factory not initialized and failed to auto-initialize: " + factoryName);
            return std::any();
        }
    }
    
    auto factoryIt = pImpl->factories.find(factoryName);
    if (factoryIt == pImpl->factories.end()) {
        NLM_LOG_ERROR("Factory not found: " + factoryName);
        return std::any();
    }
    
    auto factory = factoryIt->second;
    
    NLM_LOG_INFO("Creating object of type " + objectType + " using factory " + factoryName);
    
    try {
        std::any result = factory->create(objectType, params);
        pImpl->createCount++;
        NLM_LOG_INFO("Object created successfully: " + objectType + " via " + factoryName);
        return result;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating object " + objectType + " via factory " + factoryName + ": " + std::string(e.what()));
        pImpl->errorCount++;
        return std::any();
    }
}

std::vector<std::string> FactoryRegistry::getFactoryNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->factories.size());
    for (const auto& pair : pImpl->factories) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> FactoryRegistry::getFactoryNames(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->factoryMetadata) {
        if (pair.second.metadata.find("category") != pair.second.metadata.end() &&
            pair.second.metadata.at("category") == category) {
            names.push_back(pair.first);
        }
    }
    return names;
}

FactoryMetadata FactoryRegistry::getFactoryMetadata(const std::string& factoryName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factoryMetadata.find(factoryName);
    if (it == pImpl->factoryMetadata.end()) {
        NLM_LOG_ERROR("Factory metadata not found: " + factoryName);
        return FactoryMetadata();
    }
    
    return it->second;
}

bool FactoryRegistry::registerPool(const std::string& poolName, std::shared_ptr<IObjectPool> pool) {
    if (!pool) {
        NLM_LOG_ERROR("Cannot register null pool: " + poolName);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->pools[poolName] = pool;
    NLM_LOG_INFO("Object pool registered: " + poolName);
    return true;
}

std::shared_ptr<IObjectPool> FactoryRegistry::getPool(const std::string& poolName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pools.find(poolName);
    if (it == pImpl->pools.end()) {
        NLM_LOG_ERROR("Pool not found: " + poolName);
        return nullptr;
    }
    
    return it->second;
}

std::any FactoryRegistry::acquireObject(const std::string& poolName, const std::map<std::string, std::any>& params) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pools.find(poolName);
    if (it == pImpl->pools.end()) {
        NLM_LOG_ERROR("Pool not found: " + poolName);
        return std::any();
    }
    
    auto pool = it->second;
    NLM_LOG_INFO("Acquiring object from pool: " + poolName);
    
    try {
        std::any result = pool->acquire(params);
        NLM_LOG_INFO("Object acquired successfully from pool: " + poolName);
        return result;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception acquiring object from pool " + poolName + ": " + std::string(e.what()));
        return std::any();
    }
}

bool FactoryRegistry::releaseObject(const std::string& poolName, std::any object) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pools.find(poolName);
    if (it == pImpl->pools.end()) {
        NLM_LOG_ERROR("Pool not found: " + poolName);
        return false;
    }
    
    auto pool = it->second;
    NLM_LOG_INFO("Releasing object back to pool: " + poolName);
    
    try {
        bool result = pool->release(object);
        if (result) {
            NLM_LOG_INFO("Object released successfully to pool: " + poolName);
        }
        return result;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception releasing object to pool " + poolName + ": " + std::string(e.what()));
        return false;
    }
}

bool FactoryRegistry::registerFactoryMethod(const std::string& factoryName, const std::string& objectType,
                                           std::function<std::any(const std::map<std::string, std::any>&)> method) {
    if (!method) {
        NLM_LOG_ERROR("Cannot register null method for factory " + factoryName + " object type " + objectType);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->factoryMethods[factoryName][objectType] = method;
    NLM_LOG_INFO("Factory method registered: " + factoryName + " -> " + objectType);
    return true;
}

FactoryRegistry& FactoryRegistry::getInstance() {
    static FactoryRegistry instance;
    return instance;
}

void FactoryRegistry::reset() {
    static FactoryRegistry instance;
    // Clean up all factories
    for (const auto& pair : instance.pImpl->factories) {
        instance.unregisterFactory(pair.first);
    }
    instance.pImpl->factories.clear();
    instance.pImpl->factoryMetadata.clear();
    instance.pImpl->factoryInitialized.clear();
    instance.pImpl->factoryMethods.clear();
    instance.pImpl->pools.clear();
    instance.pImpl->registerCount = 0;
    instance.pImpl->initializeCount = 0;
    instance.pImpl->createCount = 0;
    instance.pImpl->errorCount = 0;
}

struct StaticFactory::Impl {
    std::string factoryName;
    std::string description;
};

StaticFactory::StaticFactory(const std::string& name, const std::string& description)
    : metadata_(name, description, FactoryType::Static) {
    pImpl = std::make_unique<Impl>();
    pImpl->factoryName = name;
    pImpl->description = description;
    NLM_LOG_INFO("StaticFactory created: " + name);
}

StaticFactory::~StaticFactory() {
    NLM_LOG_INFO("StaticFactory destroyed: " + pImpl->factoryName);
}

bool StaticFactory::initialize() {
    NLM_LOG_INFO("StaticFactory initializing: " + pImpl->factoryName);
    initialized_ = true;
    return true;
}

void StaticFactory::shutdown() {
    NLM_LOG_INFO("StaticFactory shutting down: " + pImpl->factoryName);
    initialized_ = false;
    creators_.clear();
}

std::any StaticFactory::create(const std::string& objectType, const std::map<std::string, std::any>& params) {
    if (!canCreate(objectType)) {
        NLM_LOG_ERROR("StaticFactory cannot create object type: " + objectType);
        throw std::runtime_error("Cannot create object type: " + objectType);
    }
    
    auto it = creators_.find(objectType);
    if (it == creators_.end()) {
        NLM_LOG_ERROR("No creator registered for object type: " + objectType);
        throw std::runtime_error("No creator registered for: " + objectType);
    }
    
    NLM_LOG_INFO("StaticFactory creating object: " + objectType);
    return it->second(params);
}

bool StaticFactory::canCreate(const std::string& objectType) const {
    return creators_.find(objectType) != creators_.end();
}

struct DynamicFactory::Impl {
    std::string factoryName;
    std::string description;
    void* handle_;
};

DynamicFactory::DynamicFactory(const std::string& name, const std::string& description)
    : metadata_(name, description, FactoryType::Dynamic) {
    pImpl = std::make_unique<Impl>();
    pImpl->factoryName = name;
    pImpl->description = description;
    pImpl->handle_ = nullptr;
    NLM_LOG_INFO("DynamicFactory created: " + name);
}

DynamicFactory::~DynamicFactory() {
    shutdown();
    NLM_LOG_INFO("DynamicFactory destroyed: " + pImpl->factoryName);
}

bool DynamicFactory::initialize() {
    NLM_LOG_INFO("DynamicFactory initializing: " + pImpl->factoryName);
    initialized_ = true;
    return true;
}

void DynamicFactory::shutdown() {
    NLM_LOG_INFO("DynamicFactory shutting down: " + pImpl->factoryName);
    initialized_ = false;
    
    if (pImpl->handle_) {
        // Unload dynamic library (platform-specific)
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(pImpl->handle_));
#else
        dlclose(pImpl->handle_);
#endif
        pImpl->handle_ = nullptr;
    }
}

std::any DynamicFactory::create(const std::string& objectType, const std::map<std::string, std::any>& params) {
    if (!canCreate(objectType)) {
        NLM_LOG_ERROR("DynamicFactory cannot create object type: " + objectType);
        throw std::runtime_error("Cannot create object type: " + objectType);
    }
    
    auto it = objectTypeToSymbol.find(objectType);
    if (it == objectTypeToSymbol.end()) {
        NLM_LOG_ERROR("No symbol found for object type: " + objectType);
        throw std::runtime_error("No symbol found for: " + objectType);
    }
    
    // This would normally resolve the symbol and call it
    // For now, just throw an exception
    NLM_LOG_ERROR("Dynamic factory creation not fully implemented for: " + objectType);
    throw std::runtime_error("Dynamic factory creation not implemented");
}

bool DynamicFactory::canCreate(const std::string& objectType) const {
    return objectTypeToSymbol.find(objectType) != objectTypeToSymbol.end();
}

bool DynamicFactory::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Loading dynamic factory from file: " + filepath);
    
    // This would normally load the shared library
    // For now, just return false
    NLM_LOG_ERROR("Dynamic factory file loading not implemented");
    return false;
}

bool DynamicFactory::loadFromSymbol(const std::string& symbolName) {
    NLM_LOG_INFO("Loading dynamic factory from symbol: " + symbolName);
    
    // This would normally load the symbol from memory
    // For now, just return false
    NLM_LOG_ERROR("Dynamic factory symbol loading not implemented");
    return false;
}

struct FactorySystem::Impl {
    std::unique_ptr<IFactoryRegistry> registry;
    bool initialized = false;
    std::mutex mutex;
};

FactorySystem::FactorySystem() : registry_(FactoryRegistry::getInstance()) {
    pImpl = std::make_unique<Impl>();
    pImpl->registry = &registry_;
    NLM_LOG_INFO("FactorySystem created");
}

FactorySystem::~FactorySystem() {
    shutdown();
    NLM_LOG_INFO("FactorySystem destroyed");
}

bool FactorySystem::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        NLM_LOG_INFO("FactorySystem already initialized");
        return true;
    }
    
    NLM_LOG_INFO("Initializing FactorySystem");
    
    try {
        // Initialize all registered factories
        auto factoryNames = registry_.getFactoryNames();
        for (const auto& name : factoryNames) {
            registry_.initializeFactory(name);
        }
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("FactorySystem initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing FactorySystem: " + std::string(e.what()));
        return false;
    }
}

void FactorySystem::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down FactorySystem");
    
    try {
        // Shutdown all factories
        auto factoryNames = registry_.getFactoryNames();
        for (const auto& name : factoryNames) {
            registry_.shutdownFactory(name);
        }
        
        pImpl->initialized = false;
        NLM_LOG_INFO("FactorySystem shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down FactorySystem: " + std::string(e.what()));
    }
}

std::any FactorySystem::getObject(const std::string& objectType, const std::map<std::string, std::any>& params) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        NLM_LOG_ERROR("FactorySystem not initialized");
        return std::any();
    }
    
    // Find factory that can create this object type
    auto factoryNames = registry_.getFactoryNames();
    for (const auto& factoryName : factoryNames) {
        std::any result = registry_.createObject(factoryName, objectType, params);
        if (!result.has_value()) {
            continue;
        }
        
        NLM_LOG_INFO("Object created: " + objectType + " using factory " + factoryName);
        return result;
    }
    
    NLM_LOG_ERROR("No factory found to create object type: " + objectType);
    return std::any();
}

bool FactorySystem::canCreateObject(const std::string& objectType) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return false;
    }
    
    auto factoryNames = registry_.getFactoryNames();
    for (const auto& factoryName : factoryNames) {
        FactoryMetadata metadata = registry_.getFactoryMetadata(factoryName);
        if (metadata.type == FactoryType::Static || metadata.type == FactoryType::Abstract) {
            // For static and abstract factories, we would check if they can create this type
            // For simplicity, just return true if factory exists
            return true;
        }
    }
    
    return false;
}

FactorySystem& FactorySystem::getInstance() {
    static FactorySystem instance;
    return instance;
}

void FactorySystem::reset() {
    static FactorySystem instance;
    instance.shutdown();
    FactoryRegistry::reset();
}

} // namespace nlm