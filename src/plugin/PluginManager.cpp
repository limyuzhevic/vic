#include "PluginManager.hpp"
#include "NLMPlugin.hpp"
#include "PluginRegistry.hpp"
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <sstream>

namespace nlm {

// Singleton implementation for PluginManager
PluginManager& PluginManager::getInstance() {
    static PluginManager instance;
    return instance;
}

bool PluginManager::initialize() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (initialized) {
        return true;
    }
    
    std::cout << "PluginManager: Initializing plugin system" << std::endl;
    
    // Load default plugins (from build/plugins directory)
    discoverPlugins();
    
    // Load plugin configurations
    loadPluginConfigurations();
    
    initialized = true;
    std::cout << "PluginManager: Plugin system initialized successfully" << std::endl;
    return true;
}

void PluginManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!initialized) {
        return;
    }
    
    std::cout << "PluginManager: Shutting down plugin system" << std::endl;
    
    // Unload all plugins
    for (auto it = plugins.begin(); it != plugins.end();) {
        unloadPlugin(it->first);
        it = plugins.begin(); // Restart because iterator is invalidated
    }
    
    savePluginConfigurations();
    initialized = false;
    std::cout << "PluginManager: Plugin system shutdown complete" << std::endl;
}

bool PluginManager::isInitialized() const {
    std::lock_guard<std::mutex> lock(mutex);
    return initialized;
}

bool PluginManager::loadPlugin(const std::string& filepath) {
    if (!isInitialized()) {
        std::cout << "PluginManager: Plugin system not initialized, cannot load plugin" << std::endl;
        return false;
    }
    
    std::cout << "PluginManager: Loading plugin from " << filepath << std::endl;
    
    // Use registry to load plugin
    PluginRegistry& registry = PluginRegistry::getInstance();
    std::unique_ptr<NLMPlugin> plugin = PluginLoader::loadPlugin(filepath);
    
    if (!plugin) {
        std::cout << "PluginManager: Failed to load plugin from " << filepath << std::endl;
        return false;
    }
    
    // Register the plugin
    if (!registry.registerPlugin(std::move(plugin))) {
        std::cout << "PluginManager: Failed to register plugin from " << filepath << std::endl;
        return false;
    }
    
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    std::cout << "PluginManager: Unloading plugin " << pluginName << std::endl;
    
    PluginRegistry& registry = PluginRegistry::getInstance();
    if (!registry.unregisterPlugin(pluginName)) {
        std::cout << "PluginManager: Failed to unregister plugin " << pluginName << std::endl;
        return false;
    }
    
    return true;
}

bool PluginManager::enablePlugin(const std::string& pluginName) {
    std::cout << "PluginManager: Enabling plugin " << pluginName << std::endl;
    
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        std::cout << "PluginManager: Plugin not found " << pluginName << std::endl;
        return false;
    }
    
    pluginStatuses[pluginName] = PluginStatus::Active;
    return true;
}

bool PluginManager::disablePlugin(const std::string& pluginName) {
    std::cout << "PluginManager:: Disabling plugin " << pluginName << std::endl;
    
    auto it = pluginStatuses.find(pluginName);
    if (it == pluginStatuses.end()) {
        std::cout << "PluginManager: Plugin status not found " << pluginName << std::endl;
        return false;
    }
    
    it->second = PluginStatus::Loaded;
    return true;
}

bool PluginManager::configurePlugin(const std::string& pluginName, const std::string& config) {
    std::cout << "PluginManager: Configuring plugin " << pluginName << " with config: " << config << std::endl;
    
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        std::cout << "PluginManager: Plugin not found " << pluginName << std::endl;
        return false;
    }
    
    if (!it->second->configure(config)) {
        std::cout << "PluginManager: Failed to configure plugin " << pluginName << std::endl;
        return false;
    }
    
    return true;
}

void PluginManager::configureAllPlugins() {
    std::cout << "PluginManager: Configuring all plugins" << std::endl;
    
    for (auto& pair : plugins) {
        if (pair.second->getStatus() == PluginStatus::Active) {
            // Use default configuration
            configurePlugin(pair.first, "");
        }
    }
}

std::unique_ptr<NLMPlugin> PluginManager::getPlugin(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        std::cout << "PluginManager: Plugin not found " << pluginName << std::endl;
        return nullptr;
    }
    
    return std::unique_ptr<NLMPlugin>(it->second.get());
}

std::vector<std::string> PluginManager::getPluginNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::string> result;
    for (const auto& pair : plugins) {
        result.push_back(pair.first);
    }
    return result;
}

PluginStatus PluginManager::getPluginStatus(const std::string& pluginName) const {
    auto it = pluginStatuses.find(pluginName);
    if (it == pluginStatuses.end()) {
        return PluginStatus::Unknown;
    }
    return it->second;
}

void* PluginManager::getRegionFactory(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return nullptr;
    }
    return it->second->getRegionFactory();
}

void* PluginManager::getNeuronFactory(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return nullptr;
    }
    return it->second->getNeuronFactory();
}

void* PluginManager::getPlasticityFactory(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return nullptr;
    }
    return it->second->getPlasticityFactory();
}

void* PluginManager::getSensoryFactory(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return nullptr;
    }
    return it->second->getSensoryFactory();
}

void* PluginManager::getMemoryFactory(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return nullptr;
    }
    return it->second->getMemoryFactory();
}

void* PluginManager::getNeuromodulationFactory(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return nullptr;
    }
    return it->second->getNeuromodulationFactory();
}

void PluginManager::loadPluginConfigurations() {
    std::cout << "PluginManager: Loading plugin configurations" << std::endl;
    
    // In a real implementation, this would load configurations from files
    // For now, it's a placeholder
}

void PluginManager::savePluginConfigurations() const {
    std::cout << "PluginManager: Saving plugin configurations" << std::endl;
    
    // In a real implementation, this would save configurations to files
    // For now, it's a placeholder
}

} // namespace nlm