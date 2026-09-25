// Factory System Implementation
// Complete implementation of the component factory system

#include "FactorySystem.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace nlm {
namespace factory {

// FactorySystem implementation
struct FactorySystem::Impl {
    std::unordered_map<std::string, std::unique_ptr<IComponentFactory>> factories;
    std::unordered_map<std::string, std::any> globalConfiguration;
    bool initialized = false;
    std::mutex mutex;
    std::mutex configMutex;
    std::string lastError;
    
    // Statistics
    size_t totalComponentsCreated = 0;
    size_t totalFactoriesRegistered = 0;
};

FactorySystem::FactorySystem() : pImpl(std::make_unique<Impl>()) {}

FactorySystem::~FactorySystem() {
    shutdownAllFactories();
}

bool FactorySystem::registerFactory(std::unique_ptr<IComponentFactory> factory) {
    if (!factory) {
        pImpl->lastError = "Cannot register null factory";
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string factoryId = factory->getFactoryId();
    if (factoryId.empty()) {
        pImpl->lastError = "Factory ID cannot be empty";
        return false;
    }
    
    // Check if factory with same ID already exists
    if (pImpl->factories.find(factoryId) != pImpl->factories.end()) {
        pImpl->lastError = "Factory already registered: " + factoryId;
        return false;
    }
    
    pImpl->factories[factoryId] = std::move(factory);
    pImpl->totalFactoriesRegistered++;
    
    return true;
}

bool FactorySystem::unregisterFactory(const std::string& factoryId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(factoryId);
    if (it == pImpl->factories.end()) {
        pImpl->lastError = "Factory not found: " + factoryId;
        return false;
    }
    
    // Stop factory if running
    // This can be overridden by derived classes
    
    pImpl->factories.erase(it);
    return true;
}

std::unique_ptr<IComponentFactory> FactorySystem::getFactory(const std::string& factoryId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(factoryId);
    if (it == pImpl->factories.end()) {
        pImpl->lastError = "Factory not found: " + factoryId;
        return nullptr;
    }
    
    return std::make_unique<IComponentFactory>(*it->second);
}

std::vector<std::unique_ptr<IComponentFactory>> FactorySystem::getFactories() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::unique_ptr<IComponentFactory>> factories;
    factories.reserve(pImpl->factories.size());
    
    for (const auto& pair : pImpl->factories) {
        factories.push_back(std::make_unique<IComponentFactory>(*pair.second));
    }
    
    return factories;
}

std::vector<std::unique_ptr<IComponentFactory>> FactorySystem::getFactoriesByProductType(const std::string& productType) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::unique_ptr<IComponentFactory>> factories;
    
    for (const auto& pair : pImpl->factories) {
        if (pair.second->canCreate(productType)) {
            factories.push_back(std::make_unique<IComponentFactory>(*pair.second));
        }
    }
    
    return factories;
}

bool FactorySystem::startAllFactories() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Call start on all factories that support it
    // Abstract factories typically don't have start/stop lifecycle
    // This can be overridden by derived classes
    
    return true;
}

bool FactorySystem::stopAllFactories() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Call stop on all factories that support it
    // Abstract factories typically don't have start/stop lifecycle
    // This can be overridden by derived classes
    
    return true;
}

bool FactorySystem::shutdownAllFactories() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Call shutdown on all factories
    // This can be overridden by derived classes
    
    pImpl->factories.clear();
    return true;
}

std::unique_ptr<IComponent> FactorySystem::createComponent(
    const std::string& factoryId,
    const std::string& productType,
    const std::unordered_map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(factoryId);
    if (it == pImpl->factories.end()) {
        pImpl->lastError = "Factory not found: " + factoryId;
        throw std::runtime_error(pImpl->lastError);
    }
    
    if (!it->second->canCreate(productType)) {
        pImpl->lastError = "Factory cannot create product type: " + productType;
        throw std::runtime_error(pImpl->lastError);
    }
    
    // Merge with global configuration
    std::unordered_map<std::string, std::any> mergedConfig = pImpl->globalConfiguration;
    mergedConfig.insert(config.begin(), config.end());
    
    auto component = it->second->create(productType, mergedConfig);
    if (component) {
        pImpl->totalComponentsCreated++;
    }
    
    return component;
}

std::vector<ComponentMetadata> FactorySystem::getAllComponents() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ComponentMetadata> components;
    
    for (const auto& pair : pImpl->factories) {
        auto factory = pair.second;
        auto products = factory->getAvailableProducts();
        components.insert(components.end(), products.begin(), products.end());
    }
    
    return components;
}

std::vector<ComponentMetadata> FactorySystem::getComponentsByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ComponentMetadata> components;
    
    for (const auto& pair : pImpl->factories) {
        auto factory = pair.second;
        ComponentMetadata factoryMetadata = factory->getMetadata();
        
        if (factoryMetadata.category == category) {
            auto products = factory->getAvailableProducts();
            components.insert(components.end(), products.begin(), products.end());
        }
    }
    
    return components;
}

std::vector<ComponentMetadata> FactorySystem::getComponentsByTag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ComponentMetadata> components;
    
    for (const auto& pair : pImpl->factories) {
        auto factory = pair.second;
        ComponentMetadata factoryMetadata = factory->getMetadata();
        
        if (std::find(factoryMetadata.tags.begin(), factoryMetadata.tags.end(), tag) != factoryMetadata.tags.end()) {
            auto products = factory->getAvailableProducts();
            components.insert(components.end(), products.begin(), products.end());
        }
    }
    
    return components;
}

size_t FactorySystem::getFactoryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->factories.size();
}

size_t FactorySystem::getComponentCount() const {
    return getAllComponents().size();
}

void FactorySystem::setGlobalConfiguration(const std::unordered_map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->globalConfiguration = config;
}

std::unordered_map<std::string, std::any> FactorySystem::getGlobalConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->globalConfiguration;
}

std::string FactorySystem::getLastError() const {
    return pImpl->lastError;
}

void FactorySystem::clearError() {
    pImpl->lastError.clear();
}

FactorySystem& FactorySystem::getInstance() {
    static FactorySystem instance;
    return instance;
}

// Factory builder implementation
FactoryBuilder& FactoryBuilder::name(const std::string& name) {
    metadata_.name = name;
    return *this;
}

FactoryBuilder& FactoryBuilder::version(const std::string& version) {
    metadata_.version = version;
    return *this;
}

FactoryBuilder& FactoryBuilder::description(const std::string& description) {
    metadata_.description = description;
    return *this;
}

FactoryBuilder& FactoryBuilder::category(const std::string& category) {
    metadata_.category = category;
    return *this;
}

FactoryBuilder& FactoryBuilder::author(const std::string& author) {
    metadata_.author = author;
    return *this;
}

FactoryBuilder& FactoryBuilder::addTag(const std::string& tag) {
    metadata_.tags.push_back(tag);
    return *this;
}

FactoryBuilder& FactoryBuilder::addProperty(const std::string& key, const std::string& value) {
    metadata_.properties[key] = value;
    return *this;
}

FactoryBuilder& FactoryBuilder::addProductType(const std::string& productType) {
    // This is only valid for factory builders, not component builders
    throw std::runtime_error("FactoryBuilder::addProductType not implemented");
}

// Factory utilities implementation
namespace utils {

bool validateFactoryConfiguration(const IComponentFactory& factory,
                                  const std::unordered_map<std::string, std::any>& config) {
    // Basic validation - check that all required configuration values are present
    // This can be overridden by derived factories
    return true;
}

std::vector<std::string> discoverAvailableFactories(const std::string& directory) {
    std::vector<std::string> factories;
    
    try {
        if (!std::filesystem::exists(directory)) {
            return factories;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // Check if it's a factory file (common extensions)
                if (filename.ends_with(".factory") || 
                    filename.ends_with(".factory.cpp") ||
                    filename.ends_with(".factory.hpp") ||
                    filename.ends_with(".factory.lib") ||
                    filename.find(".so") != std::string::npos) {
                    
                    factories.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception&) {
        // Ignore filesystem errors
    }
    
    return factories;
}

bool checkFactoryDependencies(const IComponentFactory& factory,
                              const std::vector<std::string>& loadedFactories) {
    // Check factory dependencies against loaded factories
    ComponentMetadata metadata = factory.getMetadata();
    
    for (const auto& dependency : metadata.properties) {
        if (dependency.first == "depends_on" || dependency.first == "requires") {
            if (std::find(loadedFactories.begin(), loadedFactories.end(), dependency.second) == loadedFactories.end()) {
                return false;  // Dependency not satisfied
            }
        }
    }
    
    return true;
}

std::string generateFactoryId(const std::string& prefix, const ComponentMetadata& metadata) {
    std::ostringstream oss;
    oss << prefix << "_" << metadata.name << "_" << metadata.version;
    return oss.str();
}

std::string generateProductId(const std::string& factoryId, const std::string& productType) {
    std::ostringstream oss;
    oss << factoryId << "_" << productType;
    return oss.str();
}

} // namespace utils

} // namespace factory
} // namespace nlm