// PluginManager implementation
// Manages plugin lifecycle, configuration, and event system

#include "PluginInterface.hpp"
#include <dlfcn.h>  // For dynamic library loading
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>

namespace nlm {
namespace plugin {

struct PluginManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> plugins;
    std::unordered_map<std::string, std::unique_ptr<IPluginFactory>> factories;
    std::unordered_map<std::string, std::vector<PluginEventCallback>> eventCallbacks;
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> pluginConfigurations;
    
    std::unordered_map<std::string, PluginState> pluginStates;
    std::unordered_map<std::string, std::unordered_map<std::string, void*>> pluginHandles;
    
    std::string lastError;
    
    std::unordered_map<std::string, std::any> globalConfiguration;
    
    std::mutex pluginsMutex;
    std::mutex factoriesMutex;
    std::mutex eventsMutex;
    std::mutex configMutex;
    std::mutex stateMutex;
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}

PluginManager::~PluginManager() {
    shutdownAllPlugins();
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    try {
        // Check if plugin already loaded
        std::string pluginName = utils::extractPluginName(pluginPath);
        if (hasPlugin(pluginName)) {
            pImpl->lastError = "Plugin already loaded: " + pluginName;
            return false;
        }
        
        // Try to load as dynamic library first
        void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!handle) {
            // Try to find factory in registry instead
            auto factoryIt = pImpl->factories.find(pluginName);
            if (factoryIt == pImpl->factories.end()) {
                pImpl->lastError = "Cannot load plugin: " + std::string(dlerror());
                return false;
            }
            
            // Create plugin from factory
            std::unique_ptr<IPlugin> plugin = factoryIt->second->createPlugin();
            if (!plugin) {
                pImpl->lastError = "Failed to create plugin from factory: " + pluginName;
                return false;
            }
            
            std::shared_ptr<IPlugin> pluginPtr(std::move(plugin));
            pImpl->plugins[pluginName] = pluginPtr;
            pImpl->pluginStates[pluginName] = PluginState::Loaded;
            
            // Initialize plugin with global configuration
            pluginPtr->initialize(pImpl->globalConfiguration);
            
            return true;
        }
        
        // Dynamic library loading
        pImpl->pluginHandles[pluginName] = handle;
        
        // Try to find create function
        using CreatePluginFunc = std::unique_ptr<IPlugin>(*)();
        auto createFunc = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "createPlugin"));
        if (!createFunc) {
            dlclose(handle);
            pImpl->pluginHandles.erase(pluginName);
            pImpl->lastError = "Plugin create function not found: " + std::string(dlerror());
            return false;
        }
        
        // Create plugin
        std::unique_ptr<IPlugin> plugin = createFunc();
        if (!plugin) {
            dlclose(handle);
            pImpl->pluginHandles.erase(pluginName);
            pImpl->lastError = "Failed to create plugin instance";
            return false;
        }
        
        std::shared_ptr<IPlugin> pluginPtr(std::move(plugin));
        pImpl->plugins[pluginName] = pluginPtr;
        pImpl->pluginStates[pluginName] = PluginState::Loaded;
        
        // Initialize plugin with global configuration
        pluginPtr->initialize(pImpl->globalConfiguration);
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception loading plugin: " + std::string(e.what());
        return false;
    }
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    return unloadPlugin(getPlugin(pluginName));
}

bool PluginManager::unloadPlugin(std::shared_ptr<IPlugin> plugin) {
    if (!plugin) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    try {
        // Find plugin by pointer
        std::string pluginName;
        for (const auto& pair : pImpl->plugins) {
            if (pair.second.get() == plugin.get()) {
                pluginName = pair.first;
                break;
            }
        }
        
        if (pluginName.empty()) {
            pImpl->lastError = "Plugin not found in registry";
            return false;
        }
        
        // Stop plugin if running
        if (pImpl->pluginStates[pluginName] == PluginState::Running) {
            plugin->stop();
        }
        
        // Shutdown plugin
        if (!plugin->shutdown()) {
            pImpl->lastError = "Plugin shutdown failed: " + pluginName;
            return false;
        }
        
        // Unload dynamic library if applicable
        auto handleIt = pImpl->pluginHandles.find(pluginName);
        if (handleIt != pImpl->pluginHandles.end()) {
            dlclose(handleIt->second);
            pImpl->pluginHandles.erase(handleIt);
        }
        
        // Clean up plugin resources
        plugin->cleanup();
        
        // Remove from registry
        pImpl->plugins.erase(pluginName);
        pImpl->pluginStates.erase(pluginName);
        pImpl->pluginConfigurations.erase(pluginName);
        
        // Remove event callbacks
        pImpl->eventCallbacks.erase(pluginName);
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception unloading plugin: " + std::string(e.what());
        return false;
    }
}

bool PluginManager::hasPlugin(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    return pImpl->plugins.find(pluginName) != pImpl->plugins.end();
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    auto it = pImpl->plugins.find(pluginName);
    return it != pImpl->plugins.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::getPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    std::vector<std::shared_ptr<IPlugin>> plugins;
    plugins.reserve(pImpl->plugins.size());
    for (const auto& pair : pImpl->plugins) {
        plugins.push_back(pair.second);
    }
    return plugins;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::getPluginsWithCapability(PluginCapability capability) const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    std::vector<std::shared_ptr<IPlugin>> pluginsWithCapability;
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second && pair.second->hasCapability(capability)) {
            pluginsWithCapability.push_back(pair.second);
        }
    }
    
    return pluginsWithCapability;
}

bool PluginManager::startAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex);
    
    for (auto& pair : pImpl->plugins) {
        const std::string& pluginName = pair.first;
        std::shared_ptr<IPlugin>& plugin = pair.second;
        
        if (pImpl->pluginStates[pluginName] == PluginState::Loaded) {
            pImpl->pluginStates[pluginName] = PluginState::Running;
            if (!plugin->start()) {
                pImpl->pluginStates[pluginName] = PluginState::Loaded;
                pImpl->lastError = "Plugin failed to start: " + pluginName;
                return false;
            }
        }
    }
    
    return true;
}

bool PluginManager::stopAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex);
    
    // Stop plugins in reverse order (dependencies first)
    std::vector<std::pair<std::string, std::shared_ptr<IPlugin>>> plugins;
    for (const auto& pair : pImpl->plugins) {
        plugins.push_back(pair);
    }
    
    std::reverse(plugins.begin(), plugins.end());
    
    for (const auto& pair : plugins) {
        const std::string& pluginName = pair.first;
        std::shared_ptr<IPlugin>& plugin = pair.second;
        
        if (pImpl->pluginStates[pluginName] == PluginState::Running) {
            pImpl->pluginStates[pluginName] = PluginState::Stopping;
            if (!plugin->stop()) {
                pImpl->lastError = "Plugin failed to stop: " + pluginName;
                return false;
            }
            pImpl->pluginStates[pluginName] = PluginState::Loaded;
        }
    }
    
    return true;
}

bool PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex);
    
    // Shutdown plugins in reverse order
    std::vector<std::pair<std::string, std::shared_ptr<IPlugin>>> plugins;
    for (const auto& pair : pImpl->plugins) {
        plugins.push_back(pair);
    }
    
    std::reverse(plugins.begin(), plugins.end());
    
    for (const auto& pair : plugins) {
        const std::string& pluginName = pair.first;
        std::shared_ptr<IPlugin>& plugin = pair.second;
        
        if (pImpl->pluginStates[pluginName] != PluginState::Unloaded) {
            pImpl->pluginStates[pluginName] = PluginState::Unloading;
            if (!plugin->shutdown()) {
                pImpl->lastError = "Plugin failed to shutdown: " + pluginName;
                return false;
            }
            pImpl->pluginStates[pluginName] = PluginState::Unloaded;
        }
    }
    
    // Clear all plugin data
    pImpl->plugins.clear();
    pImpl->pluginStates.clear();
    pImpl->pluginConfigurations.clear();
    pImpl->eventCallbacks.clear();
    pImpl->pluginHandles.clear();
    
    return true;
}

void PluginManager::updatePluginConfiguration(const std::string& pluginName,
                                              const std::unordered_map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it != pImpl->plugins.end()) {
        pImpl->pluginConfigurations[pluginName] = config;
        it->second->updateConfiguration(config);
    }
}

std::unordered_map<std::string, std::any> PluginManager::getPluginConfiguration(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    auto it = pImpl->pluginConfigurations.find(pluginName);
    return it != pImpl->pluginConfigurations.end() ? it->second : std::unordered_map<std::string, std::any>();
}

void PluginManager::subscribeToEvents(const std::string& pluginName, const PluginEventCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->eventsMutex);
    pImpl->eventCallbacks[pluginName].push_back(callback);
}

void PluginManager::publishEvent(const PluginEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->eventsMutex);
    
    // Find plugins that should receive this event based on capabilities
    for (const auto& pair : pImpl->plugins) {
        const std::string& pluginName = pair.first;
        std::shared_ptr<IPlugin>& plugin = pair.second;
        
        // Notify plugin of event
        plugin->onEvent(event);
        
        // Also call registered callbacks for this plugin
        auto callbacksIt = pImpl->eventCallbacks.find(pluginName);
        if (callbacksIt != pImpl->eventCallbacks.end()) {
            for (const auto& callback : callbacksIt->second) {
                callback(event);
            }
        }
    }
}

std::vector<PluginMetadata> PluginManager::discoverAvailablePlugins() const {
    std::vector<PluginMetadata> plugins;
    
    // Check registry for built-in plugins
    auto& registry = PluginRegistry::getInstance();
    auto factories = registry.getAllFactories();
    
    for (const auto& factoryPair : factories) {
        plugins.push_back(factoryPair.second->getMetadata());
    }
    
    // Could also scan filesystem for external plugins
    // For now, just return built-in plugins
    
    return plugins;
}

bool PluginManager::registerFactory(const std::string& name, std::unique_ptr<IPluginFactory> factory) {
    std::lock_guard<std::mutex> lock(pImpl->factoriesMutex);
    
    if (!factory || name.empty()) {
        pImpl->lastError = "Invalid factory registration";
        return false;
    }
    
    pImpl->factories[name] = std::move(factory);
    return true;
}

size_t PluginManager::getPluginCount() const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    return pImpl->plugins.size();
}

size_t PluginManager::getMemoryUsage() const {
    size_t total = 0;
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second) {
            total += pair.second->getMemoryUsage();
        }
    }
    
    return total;
}

double PluginManager::getAveragePerformanceImpact() const {
    double totalImpact = 0.0;
    size_t count = 0;
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second) {
            totalImpact += pair.second->getPerformanceImpact();
            count++;
        }
    }
    
    return count > 0 ? totalImpact / count : 0.0;
}

void PluginManager::setGlobalConfiguration(const std::unordered_map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->globalConfiguration = config;
    
    // Update all loaded plugins with new global configuration
    for (const auto& pair : pImpl->plugins) {
        if (pair.second) {
            pair.second->updateConfiguration(config);
        }
    }
}

std::unordered_map<std::string, std::any> PluginManager::getGlobalConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->globalConfiguration;
}

std::string PluginManager::getLastError() const {
    return pImpl->lastError;
}

void PluginManager::clearError() {
    pImpl->lastError.clear();
}

} // namespace plugin
} // namespace nlm