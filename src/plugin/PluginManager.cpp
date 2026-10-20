//! Plugin Manager Implementation for NLM Neural Simulation Project
//! 
//! This module implements the PluginManager class with its associated
//! methods and provides comprehensive plugin management functionality.

#include "PluginManager.hpp"
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

class PluginManager::Impl {
public:
    // Plugin storage
    std::unordered_map<std::string, std::unique_ptr<PluginBase>> plugins;
    std::unordered_map<std::string, PluginState> pluginStates;
    std::unordered_map<std::string, PluginMetadata> pluginMetadata;
    std::unordered_map<std::string, PluginConfig> pluginConfigs;
    
    // Event listeners
    std::vector<std::shared_ptr<IEventListener>> eventListeners;
    
    // Dependency tracking
    std::unordered_map<std::string, std::vector<std::string>> pluginDependencies;
    std::unordered_map<std::string, std::vector<std::string>> pluginOptionalDependencies;
    
    // Statistics
    size_t totalPlugins = 0;
    size_t enabledPlugins = 0;
    size_t failedPlugins = 0;
    double totalLoadTime = 0.0;
    std::unordered_map<std::string, double> pluginLoadTimes;
    
    // Error handling
    std::unordered_map<std::string, std::string> pluginErrors;
    std::vector<std::string> recentErrors;
    
    // Configuration
    std::shared_ptr<PluginConfiguration> configSystem;
    std::shared_ptr<PluginSecurityManager> securityManager;
    std::shared_ptr<PluginUpdateManager> updateManager;
    std::shared_ptr<HookSystem> hookSystem;
    std::shared_ptr<EventSystem> eventSystem;
    
    // System integration
    std::shared_ptr<PluginManager> selfRef;
    
    // Callbacks
    std::vector<PluginManager::PluginLoadCallback> loadCallbacks;
    std::vector<PluginManager::PluginStateChangeCallback> stateChangeCallbacks;
    std::vector<PluginManager::PluginEventCallback> eventCallbacks;
    
    // Flags and settings
    bool debugLogging = false;
    bool operationsPending = false;
    std::atomic<bool> paused{false};
    
    // Constructor
    Impl() : totalPlugins(0), enabledPlugins(0), failedPlugins(0), totalLoadTime(0.0) {}
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {
    initializeImpl();
}

PluginManager::~PluginManager() {
    shutdownAllPlugins();
    cleanupImpl();
}

void PluginManager::initializeImpl() {
    // Initialize plugin system
    // This would typically set up plugin discovery, etc.
}

void PluginManager::cleanupImpl() {
    // Cleanup plugin system
    pImpl->plugins.clear();
    pImpl->pluginStates.clear();
    pImpl->pluginMetadata.clear();
    pImpl->pluginConfigs.clear();
    pImpl->eventListeners.clear();
    pImpl->loadCallbacks.clear();
    pImpl->stateChangeCallbacks.clear();
    pImpl->eventCallbacks.clear();
}

bool PluginManager::loadPlugin(const std::string& filePath) {
    std::string pluginName = PluginUtils::extractPluginNameFromPath(filePath);
    
    withPluginWriteLock([&]() {
        pImpl->operationsPending = true;
    });
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Check if already loaded
        if (isPluginLoaded(pluginName)) {
            return true; // Already loaded
        }
        
        // Validate plugin file
        if (!validatePluginFile(filePath)) {
            return false;
        }
        
        // Load plugin using factory
        auto plugin = PluginUtils::loadPluginFromLibrary<PluginBase>(filePath);
        if (!plugin) {
            return false;
        }
        
        // Get metadata from plugin
        const PluginMetadata& metadata = plugin->getMetadata();
        
        // Check security permissions
        if (securityManager && !securityManager->checkSecurityPermissions(metadata.name)) {
            return false;
        }
        
        // Register with registry
        if (registerPluginWithRegistry(metadata)) {
            // Store plugin
            pImpl->plugins[pluginName] = std::move(plugin);
            pImpl->pluginStates[pluginName] = PluginState::Loaded;
            pImpl->pluginMetadata[pluginName] = metadata;
            
            // Set default config if available
            if (configSystem) {
                PluginConfig config;
                config.name = metadata.name;
                config.description = metadata.description;
                pImpl->pluginConfigs[pluginName] = config;
            }
            
            // Update statistics
            totalPlugins++;
            if (metadata.enabled) {
                enabledPlugins++;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            pImpl->pluginLoadTimes[pluginName] = duration;
            pImpl->totalLoadTime += duration;
            
            // Call load callback
            for (const auto& callback : pImpl->loadCallbacks) {
                callback(pluginName, true, "");
            }
            
            return true;
        }
    } catch (const std::exception& e) {
        // Handle error
        pImpl->pluginErrors[pluginName] = e.what();
        pImpl->recentErrors.push_back(e.what());
        pImpl->failedPlugins++;
        
        // Call load callback with error
        for (const auto& callback : pImpl->loadCallbacks) {
            callback(pluginName, false, e.what());
        }
    }
    
    withPluginWriteLock([&]() {
        pImpl->operationsPending = false;
    });
    
    return false;
}

bool PluginManager::loadPlugin(const PluginMetadata& metadata, std::unique_ptr<PluginBase> plugin) {
    std::string pluginName = metadata.name;
    
    withPluginWriteLock([&]() {
        pImpl->operationsPending = true;
    });
    
    try {
        // Check if already loaded
        if (isPluginLoaded(pluginName)) {
            return true; // Already loaded
        }
        
        // Check security permissions
        if (securityManager && !securityManager->checkSecurityPermissions(pluginName)) {
            return false;
        }
        
        // Register with registry
        if (registerPluginWithRegistry(metadata)) {
            // Store plugin
            pImpl->plugins[pluginName] = std::move(plugin);
            pImpl->pluginStates[pluginName] = PluginState::Loaded;
            pImpl->pluginMetadata[pluginName] = metadata;
            
            // Set default config if available
            if (configSystem) {
                PluginConfig config;
                config.name = metadata.name;
                config.description = metadata.description;
                pImpl->pluginConfigs[pluginName] = config;
            }
            
            // Update statistics
            totalPlugins++;
            if (metadata.enabled) {
                enabledPlugins++;
            }
            
            // Call load callback
            for (const auto& callback : pImpl->loadCallbacks) {
                callback(pluginName, true, "");
            }
            
            return true;
        }
    } catch (const std::exception& e) {
        // Handle error
        pImpl->pluginErrors[pluginName] = e.what();
        pImpl->recentErrors.push_back(e.what());
        pImpl->failedPlugins++;
        
        // Call load callback with error
        for (const auto& callback : pImpl->loadCallbacks) {
            callback(pluginName, false, e.what());
        }
    }
    
    withPluginWriteLock([&]() {
        pImpl->operationsPending = false;
    });
    
    return false;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    withPluginWriteLock([&]() {
        if (pImpl->plugins.find(pluginName) == pImpl->plugins.end()) {
            return false;
        }
        
        pImpl->operationsPending = true;
        PluginState currentState = pImpl->pluginStates[pluginName];
        
        // If running, stop first
        if (currentState == PluginState::Running) {
            stopPlugin(pluginName);
        }
        
        // If initialized, shutdown
        if (currentState == PluginState::Initialized || currentState == PluginState::Running) {
            shutdownPlugin(pluginName);
        }
        
        // Call unload callback
        for (const auto& callback : pImpl->loadCallbacks) {
            callback(pluginName, false, "Unloading plugin");
        }
        
        // Remove from registry
        if (unregisterPluginFromRegistry(pluginName)) {
            // Remove from storage
            pImpl->plugins.erase(pluginName);
            pImpl->pluginStates.erase(pluginName);
            pImpl->pluginMetadata.erase(pluginName);
            pImpl->pluginConfigs.erase(pluginName);
            
            // Update statistics
            totalPlugins--;
            if (pImpl->pluginMetadata.find(pluginName) != pImpl->pluginMetadata.end() && 
                pImpl->pluginMetadata[pluginName].enabled) {
                enabledPlugins--;
            }
            
            // Clear errors
            pImpl->pluginErrors.erase(pluginName);
            
            return true;
        }
        
        return false;
    });
    
    withPluginWriteLock([&]() {
        pImpl->operationsPending = false;
    });
    
    return false;
}

bool PluginManager::enablePlugin(const std::string& pluginName) {
    withPluginWriteLock([&]() {
        auto it = pImpl->pluginMetadata.find(pluginName);
        if (it == pImpl->pluginMetadata.end()) {
            return false;
        }
        
        it->second.enabled = true;
        enabledPlugins++;
        
        // Update registry
        if (pImpl->selfRef) {
            pImpl->selfRef->registerPluginWithRegistry(it->second);
        }
        
        return true;
    });
    
    return false;
}

bool PluginManager::disablePlugin(const std::string& pluginName) {
    withPluginWriteLock([&]() {
        auto it = pImpl->pluginMetadata.find(pluginName);
        if (it == pImpl->pluginMetadata.end()) {
            return false;
        }
        
        if (it->second.enabled) {
            enabledPlugins--;
        }
        
        it->second.enabled = false;
        
        // Update registry
        if (pImpl->selfRef) {
            pImpl->selfRef->registerPluginWithRegistry(it->second);
        }
        
        return true;
    });
    
    return false;
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    return withPluginLock([&]() {
        std::vector<std::string> pluginNames;
        pluginNames.reserve(pImpl->plugins.size());
        for (const auto& pair : pImpl->plugins) {
            pluginNames.push_back(pair.first);
        }
        return pluginNames;
    });
}

std::vector<std::string> PluginManager::getEnabledPlugins() const {
    return withPluginLock([&]() {
        std::vector<std::string> enabledPlugins;
        for (const auto& pair : pImpl->pluginMetadata) {
            if (pair.second.enabled) {
                enabledPlugins.push_back(pair.first);
            }
        }
        return enabledPlugins;
    });
}

PluginState PluginManager::getPluginState(const std::string& pluginName) const {
    return withPluginLock([&]() {
        auto it = pImpl->pluginStates.find(pluginName);
        return it != pImpl->pluginStates.end() ? it->second : PluginState::Unloaded;
    });
}

const PluginMetadata* PluginManager::getPluginMetadata(const std::string& pluginName) const {
    return withPluginLock([&]() {
        auto it = pImpl->pluginMetadata.find(pluginName);
        return it != pImpl->pluginMetadata.end() ? &it->second : nullptr;
    });
}

const PluginConfig* PluginManager::getPluginConfig(const std::string& pluginName) const {
    return withPluginLock([&]() {
        auto it = pImpl->pluginConfigs.find(pluginName);
        return it != pImpl->pluginConfigs.end() ? &it->second : nullptr;
    });
}

PluginBase* PluginManager::getPlugin(const std::string& pluginName) {
    return withPluginLock([&]() {
        auto it = pImpl->plugins.find(pluginName);
        return it != pImpl->plugins.end() ? it->second.get() : nullptr;
    });
}

bool PluginManager::initializePlugin(const std::string& pluginName) {
    PluginBase* plugin = getPlugin(pluginName);
    if (!plugin) {
        return false;
    }
    
    const PluginConfig& config = getPluginConfig(pluginName);
    
    withPluginWriteLock([&]() {
        auto it = pImpl->pluginStates.find(pluginName);
        if (it == pImpl->pluginStates.end() || it->second != PluginState::Loaded) {
            return false;
        }
        
        it->second = PluginState::Initializing;
    });
    
    bool success = plugin->load(config);
    
    withPluginWriteLock([&]() {
        auto it = pImpl->pluginStates.find(pluginName);
        if (it == pImpl->pluginStates.end()) {
            return;
        }
        
        if (success) {
            it->second = PluginState::Initialized;
        } else {
            it->second = PluginState::Failed;
            pImpl->failedPlugins++;
        }
    });
    
    return success;
}

void PluginManager::startAllPlugins() {
    for (const auto& pluginName : getEnabledPlugins()) {
        startAllPlugins();
    }
}

void PluginManager::updateAllPlugins(double deltaTime) {
    for (const auto& pluginName : getEnabledPlugins()) {
        PluginBase* plugin = getPlugin(pluginName);
        if (plugin && plugin->getState() == PluginState::Running) {
            plugin->update(deltaTime);
        }
    }
}

void PluginManager::stopAllPlugins() {
    for (const auto& pluginName : getEnabledPlugins()) {
        stopAllPlugins();
    }
}

bool PluginManager::shutdownPlugin(const std::string& pluginName) {
    PluginBase* plugin = getPlugin(pluginName);
    if (!plugin) {
        return false;
    }
    
    withPluginWriteLock([&]() {
        auto it = pImpl->pluginStates.find(pluginName);
        if (it == pImpl->pluginStates.end() || 
            (it->second != PluginState::Initialized && it->second != PluginState::Running)) {
            return false;
        }
        
        it->second = PluginState::ShuttingDown;
    });
    
    bool success = plugin->shutdown();
    
    withPluginWriteLock([&]() {
        auto it = pImpl->pluginStates.find(pluginName);
        if (it == pImpl->pluginStates.end()) {
            return;
        }
        
        it->second = success ? PluginState::Loaded : PluginState::Failed;
    });
    
    return success;
}

bool PluginManager::resolveDependencies(const std::string& pluginName) {
    const PluginMetadata* metadata = getPluginMetadata(pluginName);
    if (!metadata) {
        return false;
    }
    
    // Check required dependencies
    for (const auto& dependency : metadata->dependencies) {
        if (!isPluginLoaded(dependency) || 
            getPluginState(dependency) != PluginState::Initialized) {
            return false; // Missing dependency
        }
    }
    
    // Check optional dependencies
    for (const auto& dependency : metadata->optionalDependencies) {
        if (!isPluginLoaded(dependency)) {
            // Optional dependency missing is OK
            continue;
        }
    }
    
    return true;
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& pluginName) const {
    return withPluginLock([&]() {
        auto it = pImpl->pluginDependencies.find(pluginName);
        return it != pImpl->pluginDependencies.end() ? it->second : std::vector<std::string>();
    });
}

std::vector<std::string> PluginManager::getUnsatisfiedDependencies(const std::string& pluginName) const {
    std::vector<std::string> unsatisfied;
    
    const PluginMetadata* metadata = getPluginMetadata(pluginName);
    if (!metadata) {
        return unsatisfied;
    }
    
    for (const auto& dependency : metadata->dependencies) {
        if (!isPluginLoaded(dependency) || 
            getPluginState(dependency) != PluginState::Initialized) {
            unsatisfied.push_back(dependency);
        }
    }
    
    return unsatisfied;
}

bool PluginManager::configurePlugin(const std::string& pluginName, const PluginConfig& config) {
    PluginBase* plugin = getPlugin(pluginName);
    if (!plugin) {
        return false;
    }
    
    withPluginWriteLock([&]() {
        pImpl->pluginConfigs[pluginName] = config;
    });
    
    return plugin->load(config);
}

bool PluginManager::updatePluginConfiguration(const std::string& pluginName, 
                                              const std::string& key, 
                                              const std::any& value) {
    PluginBase* plugin = getPlugin(pluginName);
    if (!plugin) {
        return false;
    }
    
    withPluginWriteLock([&]() {
        auto it = pImpl->pluginConfigs.find(pluginName);
        if (it == pImpl->pluginConfigs.end()) {
            return false;
        }
        
        it->second.setParameter(key, value);
    });
    
    // Update plugin configuration
    const PluginConfig& config = getPluginConfig(pluginName);
    return plugin->load(config);
}

std::string PluginManager::serializePluginConfiguration(const std::string& pluginName) const {
    const PluginConfig* config = getPluginConfig(pluginName);
    if (!config) {
        return "";
    }
    
    return config->serialize();
}

bool PluginManager::deserializePluginConfiguration(const std::string& pluginName, 
                                                   const std::string& data) {
    PluginBase* plugin = getPlugin(pluginName);
    if (!plugin) {
        return false;
    }
    
    PluginConfig config;
    if (!config.deserialize(data)) {
        return false;
    }
    
    return configurePlugin(pluginName, config);
}

void PluginManager::addEventListener(std::shared_ptr<IEventListener> listener) {
    withPluginWriteLock([&]() {
        pImpl->eventListeners.push_back(listener);
    });
}

void PluginManager::removeEventListener(const std::string& listenerName) {
    withPluginWriteLock([&]() {
        pImpl->eventListeners.erase(
            std::remove_if(pImpl->eventListeners.begin(), pImpl->eventListeners.end(),
                          [&listenerName](const std::shared_ptr<IEventListener>& listener) {
                              return listener->getName() == listenerName;
                          }),
            pImpl->eventListeners.end()
        );
    });
}

void PluginManager::notifyEvent(const PluginEventData& event) {
    // Notify all event listeners
    for (const auto& listener : pImpl->eventListeners) {
        try {
            listener->onPluginEvent(event);
        } catch (const std::exception& e) {
            // Handle error
        }
    }
    
    // Call event callbacks
    for (const auto& callback : pImpl->eventCallbacks) {
        try {
            callback(event);
        } catch (const std::exception& e) {
            // Handle error
        }
    }
}

void PluginManager::notifyHook(const HookEventData& hook) {
    // Notify all event listeners
    for (const auto& listener : pImpl->eventListeners) {
        try {
            listener->onHookEvent(hook);
        } catch (const std::exception& e) {
            // Handle error
        }
    }
}

bool PluginManager::registerPluginWithRegistry(const PluginMetadata& metadata) {
    if (pImpl->selfRef) {
        return pImpl->selfRef->registerPlugin(metadata);
    }
    return false;
}

bool PluginManager::unregisterPluginFromRegistry(const std::string& pluginName) {
    if (pImpl->selfRef) {
        return pImpl->selfRef->unregisterPlugin(pluginName);
    }
    return false;
}

IPluginRegistry* PluginManager::getRegistry() const {
    // Return registry if available
    return nullptr; // Placeholder
}

bool PluginManager::checkSecurityPermissions(const std::string& pluginName) const {
    if (pImpl->securityManager) {
        return pImpl->securityManager->checkSecurityPermissions(pluginName);
    }
    return true;
}

std::vector<std::string> PluginManager::getRequiredPermissions(const std::string& pluginName) const {
    if (pImpl->securityManager) {
        return pImpl->securityManager->getRequiredPermissions(pluginName);
    }
    return {};
}

std::vector<std::string> PluginManager::getGrantedPermissions(const std::string& pluginName) const {
    if (pImpl->securityManager) {
        return pImpl->securityManager->getGrantedPermissions(pluginName);
    }
    return {};
}

bool PluginManager::savePluginConfigurations(const std::string& filePath) const {
    // Save configurations to file
    // This would use serialization
    return false; // Placeholder
}

bool PluginManager::loadPluginConfigurations(const std::string& filePath) {
    // Load configurations from file
    // This would use deserialization
    return false; // Placeholder
}

bool PluginManager::validateAllPluginConfigurations() const {
    for (const auto& pair : pImpl->pluginConfigs) {
        if (!pImpl->configSystem) {
            continue;
        }
        
        if (!pImpl->configSystem->validateConfiguration(pair.second)) {
            return false;
        }
    }
    
    return true;
}

size_t PluginManager::getLoadedPluginCount() const {
    return withPluginLock([&]() {
        return pImpl->plugins.size();
    });
}

size_t PluginManager::getEnabledPluginCount() const {
    return withPluginLock([&]() {
        return pImpl->pluginMetadata.size();
    });
}

size_t PluginManager::getFailedPluginCount() const {
    return withPluginLock([&]() {
        return pImpl->failedPlugins;
    });
}

double PluginManager::getAveragePluginLoadTime() const {
    return withPluginLock([&]() {
        if (pImpl->pluginLoadTimes.empty()) {
            return 0.0;
        }
        return pImpl->totalLoadTime / pImpl->pluginLoadTimes.size();
    });
}

std::unordered_map<std::string, double> PluginManager::getPluginLoadTimes() const {
    return withPluginLock([&]() {
        return pImpl->pluginLoadTimes;
    });
}

std::vector<std::string> PluginManager::getPluginsByType(const std::string& pluginType) const {
    // Get plugins by type from registry
    return {}; // Placeholder
}

std::vector<std::string> PluginManager::getPluginsByTag(const std::string& tag) const {
    // Get plugins by tag from registry
    return {}; // Placeholder
}

void PluginManager::waitForPluginOperations() {
    withPluginWriteLock([&]() {
        operationsPending = true;
    });
    
    // Wait for operations to complete
    // This would use condition variables
    
    withPluginWriteLock([&]() {
        operationsPending = false;
    });
}

bool PluginManager::isOperationPending() const {
    return withPluginLock([&]() {
        return pImpl->operationsPending;
    });
}

void PluginManager::pauseAllPlugins() {
    pImpl->paused = true;
}

void PluginManager::resumeAllPlugins() {
    pImpl->paused = false;
}

bool PluginManager::shutdownAllPlugins() {
    std::vector<std::string> pluginNames = getLoadedPlugins();
    for (const auto& pluginName : pluginNames) {
        shutdownPlugin(pluginName);
    }
    
    return true;
}

void PluginManager::clearAllPlugins() {
    shutdownAllPlugins();
    
    withPluginWriteLock([&]() {
        pImpl->plugins.clear();
        pImpl->pluginStates.clear();
        pImpl->pluginMetadata.clear();
        pImpl->pluginConfigs.clear();
        pImpl->pluginDependencies.clear();
        pImpl->pluginOptionalDependencies.clear();
        pImpl->pluginLoadTimes.clear();
        pImpl->pluginErrors.clear();
        pImpl->recentErrors.clear();
        pImpl->totalPlugins = 0;
        pImpl->enabledPlugins = 0;
        pImpl->failedPlugins = 0;
        pImpl->totalLoadTime = 0.0;
    });
}

std::vector<std::string> PluginManager::discoverPlugins(const std::string& directoryPath) const {
    // Discover plugins from directory
    // This would use file system scanning
    return {}; // Placeholder
}

bool PluginManager::validatePluginFile(const std::string& filePath) const {
    // Validate plugin file
    // Check if it's a valid plugin file
    return false; // Placeholder
}

bool PluginManager::replacePlugin(const std::string& pluginName, const std::string& newFilePath) {
    // Replace plugin with new version
    // This would involve unloading old plugin and loading new one
    return false; // Placeholder
}

bool PluginManager::updatePlugin(const std::string& pluginName) {
    // Update plugin to latest version
    return false; // Placeholder
}

std::vector<std::string> PluginManager::getPluginUpdateCandidates() const {
    // Get plugins that need updating
    return {}; // Placeholder
}

std::string PluginManager::getLastError(const std::string& pluginName) const {
    withPluginLock([&]() {
        auto it = pImpl->pluginErrors.find(pluginName);
        return it != pImpl->pluginErrors.end() ? it->second : "";
    });
}

std::vector<std::string> PluginManager::getRecentErrors() const {
    return withPluginLock([&]() {
        return pImpl->recentErrors;
    });
}

void PluginManager::clearErrors(const std::string& pluginName) {
    withPluginWriteLock([&]() {
        if (!pluginName.empty()) {
            pImpl->pluginErrors.erase(pluginName);
        } else {
            pImpl->pluginErrors.clear();
            pImpl->recentErrors.clear();
        }
    });
}

bool PluginManager::invokeHook(const PluginHook& hook, HookEventData& data, const std::string& pluginName) {
    if (pImpl->hookSystem) {
        return pImpl->hookSystem->invokeHook(hook, data, pluginName);
    }
    return false;
}

PluginManager::PluginStats PluginManager::getPluginStats(const std::string& pluginName) const {
    PluginStats stats;
    stats.name = pluginName;
    
    withPluginLock([&]() {
        auto it = pImpl->pluginStates.find(pluginName);
        if (it != pImpl->pluginStates.end()) {
            stats.state = it->second;
        }
        
        auto loadIt = pImpl->pluginLoadTimes.find(pluginName);
        if (loadIt != pImpl->pluginLoadTimes.end()) {
            stats.loadTime = loadIt->second;
        }
    });
    
    return stats;
}

std::vector<PluginManager::PluginStats> PluginManager::getAllPluginStats() const {
    std::vector<PluginStats> stats;
    
    withPluginLock([&]() {
        for (const auto& pair : pImpl->plugins) {
            PluginStats stat;
            stat.name = pair.first;
            auto it = pImpl->pluginStates.find(pair.first);
            if (it != pImpl->pluginStates.end()) {
                stat.state = it->second;
            }
            auto loadIt = pImpl->pluginLoadTimes.find(pair.first);
            if (loadIt != pImpl->pluginLoadTimes.end()) {
                stat.loadTime = loadIt->second;
            }
            stats.push_back(stat);
        }
    });
    
    return stats;
}

void PluginManager::setConfigurationSystem(std::shared_ptr<PluginConfiguration> configSystem) {
    pImpl->configSystem = configSystem;
}

void PluginManager::setSecurityManager(std::shared_ptr<PluginSecurityManager> securityManager) {
    pImpl->securityManager = securityManager;
}

void PluginManager::setUpdateManager(std::shared_ptr<PluginUpdateManager> updateManager) {
    pImpl->updateManager = updateManager;
}

void PluginManager::setHookSystem(std::shared_ptr<HookSystem> hookSystem) {
    pImpl->hookSystem = hookSystem;
}

void PluginManager::setEventSystem(std::shared_ptr<EventSystem> eventSystem) {
    pImpl->eventSystem = eventSystem;
}

void PluginManager::addLoadCallback(const PluginLoadCallback& callback) {
    pImpl->loadCallbacks.push_back(callback);
}

void PluginManager::addStateChangeCallback(const PluginStateChangeCallback& callback) {
    pImpl->stateChangeCallbacks.push_back(callback);
}

void PluginManager::addEventCallback(const PluginEventCallback& callback) {
    pImpl->eventCallbacks.push_back(callback);
}

void PluginManager::enableDebugLogging(bool enable) {
    pImpl->debugLogging = enable;
}

bool PluginManager::isDebugLoggingEnabled() const {
    return pImpl->debugLogging;
}

void PluginManager::logPluginState(const std::string& pluginName, PluginState state) const {
    if (pImpl->debugLogging) {
        // Log state change
    }
}

void PluginManager::logPluginError(const std::string& pluginName, const std::string& error) const {
    if (pImpl->debugLogging) {
        // Log error
    }
}

PluginManager::PerformanceMetrics PluginManager::getPerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    withPluginLock([&]() {
        metrics.averageLoadTime = getAveragePluginLoadTime();
        metrics.totalUpdates = 0; // Would need to track
        metrics.failedLoads = pImpl->failedPlugins;
        metrics.systemUptime = 0.0; // Would need to track
    });
    
    return metrics;
}

// Helper function implementations
namespace PluginUtils {
    std::string extractPluginNameFromPath(const std::string& filePath) {
        std::filesystem::path path(filePath);
        return path.stem().string();
    }
    
    std::string getLibraryExtension() {
        return ".so"; // Linux shared library
    }
    
    std::string getPluginSymbolPrefix() {
        return "create_";
    }
    
    bool isPluginFile(const std::string& filePath) {
        std::filesystem::path path(filePath);
        std::string extension = path.extension().string();
        return extension == ".so"; // Check for shared library
    }
    
    PluginVersion parsePluginVersion(const std::string& versionString) {
        PluginVersion version;
        // Parse version string (e.g., "1.2.3")
        return version;
    }
    
    std::string getPluginTypeFromName(const std::string& pluginName) {
        // Determine plugin type from name
        // This would typically use a mapping
        return "Generic";
    }
    
    template<typename T>
    std::unique_ptr<T> loadPluginFromLibrary(const std::string& filePath) {
        // Load plugin from shared library
        // This would use dlopen, dlsym, dlclose
        return nullptr; // Placeholder
    }
}

} // namespace nlm