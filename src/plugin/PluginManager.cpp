/*
 * Plugin Architecture Implementation for NLM
 * 
 * This file contains the core plugin system implementation that enables
 * extensibility and customization of the NLM (Neural Learning Machine) framework.
 * 
 * The plugin system allows users to:
 * - Add custom sensor implementations
 * - Create specialized neural network architectures
 * - Implement alternative world simulation models
 * - Add custom neuromodulation systems
 * - Extend memory systems
 * - Integrate with external systems
 * 
 * Key features:
 * - Dynamic plugin loading and unloading
 * - Plugin lifecycle management (initialize/shutdown)
 * - Configuration system for plugins
 * - Thread-safe plugin management
 * - Error handling and recovery
 */

#include "PluginManager.h"
#include "IPlugin.h"
#include "ISensorPlugin.h"
#include "INeuralNetworkPlugin.h"
#include "IWorldPlugin.h"
#include "../core/Logger/Logger.hpp"
#include <dlfcn.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <mutex>
#include <unordered_map>

namespace nlm {

// Global plugin manager instance
static std::unique_ptr<PluginManager> g_pluginManager = nullptr;

// Plugin loading result structure
struct PluginLoadResult {
    bool success;
    std::string pluginName;
    std::string errorMessage;
    std::string version;
};

// Plugin metadata structure
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string path;
    bool enabled;
    std::unordered_map<std::string, std::string> metadata;
};

// ============================================================================
// PluginManager Implementation
// ============================================================================

PluginManager::PluginManager() : initialized_(false), shutdown_(false) {
    // Initialize internal structures
    pluginRegistry_.clear();
    pluginInstances_.clear();
    pluginConfigs_.clear();
    
    Logger::getGlobal().info("PluginManager initialized");
}

PluginManager::~PluginManager() {
    shutdownAllPlugins();
    initialized_ = false;
    
    Logger::getGlobal().info("PluginManager shutdown completed");
}

PluginManager& PluginManager::getInstance() {
    if (!g_pluginManager) {
        g_pluginManager = std::make_unique<PluginManager>();
    }
    return *g_pluginManager;
}

bool PluginManager::initialize(const Config& config) {
    if (initialized_) {
        return true; // Already initialized
    }
    
    try {
        // Load configuration
        loadConfiguration(config);
        
        // Initialize core plugins
        initializeCorePlugins();
        
        initialized_ = true;
        
        Logger::getGlobal().info("PluginManager initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error(std::string("PluginManager initialization failed: ") + e.what());
        initialized_ = false;
        return false;
    }
}

void PluginManager::shutdown() {
    if (shutdown_ || !initialized_) {
        return;
    }
    
    try {
        // Shutdown all plugins
        shutdownAllPlugins();
        
        // Clear internal structures
        pluginRegistry_.clear();
        pluginInstances_.clear();
        pluginConfigs_.clear();
        
        initialized_ = false;
        shutdown_ = true;
        
        Logger::getGlobal().info("PluginManager shutdown successfully");
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error(std::string("PluginManager shutdown failed: ") + e.what());
        throw;
    }
}

std::vector<std::string> PluginManager::getAvailablePlugins() const {
    std::vector<std::string> pluginNames;
    
    for (const auto& pair : pluginRegistry_) {
        pluginNames.push_back(pair.first);
    }
    
    std::sort(pluginNames.begin(), pluginNames.end());
    return pluginNames;
}

std::vector<std::string> PluginManager::getEnabledPlugins() const {
    std::vector<std::string> enabledPlugins;
    
    for (const auto& pair : pluginRegistry_) {
        if (pair.second.enabled) {
            enabledPlugins.push_back(pair.first);
        }
    }
    
    std::sort(enabledPlugins.begin(), enabledPlugins.end());
    return enabledPlugins;
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(pluginMutex_);
    
    try {
        // Check if plugin is already loaded
        std::string pluginName = extractPluginName(pluginPath);
        if (isPluginLoaded(pluginName)) {
            Logger::getGlobal().warning(std::string("Plugin already loaded: ") + pluginName);
            return true;
        }
        
        // Load plugin library
        void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!handle) {
            PluginLoadResult result;
            result.success = false;
            result.pluginName = pluginName;
            result.errorMessage = dlerror();
            
            logLoadResult(result);
            return false;
        }
        
        // Get plugin factory function
        using PluginFactory = std::function<std::shared_ptr<IPlugin>()>;
        PluginFactory factory = nullptr;
        
        // Try to get the factory function
        *reinterpret_cast<void**>(&factory) = dlsym(handle, "createPlugin");
        if (!factory) {
            dlclose(handle);
            
            PluginLoadResult result;
            result.success = false;
            result.pluginName = pluginName;
            result.errorMessage = dlerror();
            
            logLoadResult(result);
            return false;
        }
        
        // Create plugin instance
        auto plugin = factory();
        if (!plugin) {
            dlclose(handle);
            
            PluginLoadResult result;
            result.success = false;
            result.pluginName = pluginName;
            result.errorMessage = "Plugin factory returned null";
            
            logLoadResult(result);
            return false;
        }
        
        // Get plugin metadata
        PluginMetadata metadata;
        metadata.name = plugin->getName();
        metadata.version = plugin->getVersion();
        metadata.description = plugin->getDescription();
        metadata.path = pluginPath;
        metadata.enabled = false; // Default to disabled
        
        // Store plugin
        pluginRegistry_[metadata.name] = metadata;
        pluginInstances_[metadata.name] = std::make_pair(handle, plugin);
        
        Logger::getGlobal().info(std::string("Plugin loaded: ") + metadata.name + 
                               " v" + metadata.version + " from " + pluginPath);
        
        return true;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error(std::string("Plugin loading failed: ") + e.what());
        return false;
    }
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pluginMutex_);
    
    try {
        auto it = pluginInstances_.find(pluginName);
        if (it == pluginInstances_.end()) {
            Logger::getGlobal().warning(std::string("Plugin not found: ") + pluginName);
            return false;
        }
        
        void* handle = it->second.first;
        auto plugin = it->second.second;
        
        // Check if plugin is enabled
        if (pluginRegistry_[pluginName].enabled) {
            plugin->shutdown();
        }
        
        // Unload library
        dlclose(handle);
        
        // Remove from registries
        pluginInstances_.erase(it);
        pluginRegistry_.erase(pluginName);
        pluginConfigs_.erase(pluginName);
        
        Logger::getGlobal().info(std::string("Plugin unloaded: ") + pluginName);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error(std::string("Plugin unloading failed: ") + e.what());
        return false;
    }
}

bool PluginManager::enablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pluginMutex_);
    
    try {
        auto it = pluginInstances_.find(pluginName);
        if (it == pluginInstances_.end()) {
            Logger::getGlobal().warning(std::string("Plugin not found: ") + pluginName);
            return false;
        }
        
        if (pluginRegistry_[pluginName].enabled) {
            return true; // Already enabled
        }
        
        auto plugin = it->second.second;
        
        // Initialize plugin
        if (!plugin->initialize(getPluginConfig(pluginName))) {
            Logger::getGlobal().error(std::string("Plugin initialization failed: ") + pluginName);
            return false;
        }
        
        // Enable plugin
        plugin->setEnabled(true);
        pluginRegistry_[pluginName].enabled = true;
        
        Logger::getGlobal().info(std::string("Plugin enabled: ") + pluginName);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error(std::string("Plugin enable failed: ") + e.what());
        return false;
    }
}

bool PluginManager::disablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pluginMutex_);
    
    try {
        auto it = pluginInstances_.find(pluginName);
        if (it == pluginInstances_.end()) {
            Logger::getGlobal().warning(std::string("Plugin not found: ") + pluginName);
            return false;
        }
        
        if (!pluginRegistry_[pluginName].enabled) {
            return true; // Already disabled
        }
        
        auto plugin = it->second.second;
        
        // Disable plugin
        plugin->setEnabled(false);
        plugin->shutdown();
        pluginRegistry_[pluginName].enabled = false;
        
        Logger::getGlobal().info(std::string("Plugin disabled: ") + pluginName);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error(std::string("Plugin disable failed: ") + e.what());
        return false;
    }
}

bool PluginManager::isPluginEnabled(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pluginMutex_);
    
    auto it = pluginRegistry_.find(pluginName);
    if (it == pluginRegistry_.end()) {
        return false;
    }
    
    return it->second.enabled;
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pluginMutex_);
    
    auto it = pluginInstances_.find(pluginName);
    if (it == pluginInstances_.end()) {
        return nullptr;
    }
    
    return it->second.second;
}

std::vector<std::shared_ptr<ISensorPlugin>> PluginManager::getSensorPlugins() const {
    std::vector<std::shared_ptr<ISensorPlugin>> sensorPlugins;
    
    for (const auto& pair : pluginInstances_) {
        auto plugin = std::dynamic_pointer_cast<ISensorPlugin>(pair.second.second);
        if (plugin && plugin->isEnabled()) {
            sensorPlugins.push_back(plugin);
        }
    }
    
    return sensorPlugins;
}

std::vector<std::shared_ptr<INeuralNetworkPlugin>> PluginManager::getNeuralNetworkPlugins() const {
    std::vector<std::shared_ptr<INeuralNetworkPlugin>> nnPlugins;
    
    for (const auto& pair : pluginInstances_) {
        auto plugin = std::dynamic_pointer_cast<INeuralNetworkPlugin>(pair.second.second);
        if (plugin && plugin->isEnabled()) {
            nnPlugins.push_back(plugin);
        }
    }
    
    return nnPlugins;
}

std::vector<std::shared_ptr<IWorldPlugin>> PluginManager::getWorldPlugins() const {
    std::vector<std::shared_ptr<IWorldPlugin>> worldPlugins;
    
    for (const auto& pair : pluginInstances_) {
        auto plugin = std::dynamic_pointer_cast<IWorldPlugin>(pair.second.second);
        if (plugin && plugin->isEnabled()) {
            worldPlugins.push_back(plugin);
        }
    }
    
    return worldPlugins;
}

bool PluginManager::configurePlugin(const std::string& pluginName, const Config& config) {
    std::lock_guard<std::mutex> lock(pluginMutex_);
    
    auto it = pluginConfigs_.find(pluginName);
    if (it == pluginConfigs_.end()) {
        pluginConfigs_[pluginName] = config;
    } else {
        it->second = config;
    }
    
    return true;
}

Config PluginManager::getPluginConfig(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pluginMutex_);
    
    auto it = pluginConfigs_.find(pluginName);
    if (it == pluginConfigs_.end()) {
        return Config(); // Return empty config
    }
    
    return it->second;
}

void PluginManager::initializeAllPlugins(const Config& globalConfig) {
    // Enable all plugins that are marked as enabled in configuration
    auto enabledPlugins = getEnabledPluginsFromConfig(globalConfig);
    
    for (const auto& pluginName : enabledPlugins) {
        enablePlugin(pluginName);
    }
}

void PluginManager::shutdownAllPlugins() {
    // Disable all plugins
    auto enabledPlugins = getEnabledPlugins();
    
    for (const auto& pluginName : enabledPlugins) {
        disablePlugin(pluginName);
    }
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pluginMutex_);
    
    return pluginInstances_.find(pluginName) != pluginInstances_.end();
}

void PluginManager::loadConfiguration(const Config& config) {
    // Load plugin configuration from the main config
    if (config.has("plugins.enabled")) {
        auto enabledPlugins = config.get<std::vector<std::string>>("plugins.enabled");
        
        // Enable plugins based on configuration
        for (const auto& pluginName : enabledPlugins) {
            if (isPluginLoaded(pluginName)) {
                enablePlugin(pluginName);
            }
        }
    }
}

void PluginManager::initializeCorePlugins() {
    // Initialize any core plugins that come with NLM
    // This can include built-in sensor, neural network, and world plugins
    
    // For example:
    // loadPlugin("libnlm_core_sensors.so");
    // loadPlugin("libnlm_core_nn.so");
    // loadPlugin("libnlm_core_world.so");
}

std::vector<std::string> PluginManager::getEnabledPluginsFromConfig(const Config& config) const {
    std::vector<std::string> enabledPlugins;
    
    if (config.has("plugins.enabled")) {
        enabledPlugins = config.get<std::vector<std::string>>("plugins.enabled");
    }
    
    return enabledPlugins;
}

std::string PluginManager::extractPluginName(const std::string& pluginPath) const {
    // Extract plugin name from path
    size_t lastSlash = pluginPath.find_last_of('/');
    size_t lastDot = pluginPath.find_last_of('.');
    
    if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash) {
        return pluginPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }
    
    return pluginPath;
}

void PluginManager::logLoadResult(const PluginLoadResult& result) {
    if (result.success) {
        Logger::getGlobal().info(std::string("Plugin loaded successfully: ") + 
                               result.pluginName + " v" + result.version);
    } else {
        Logger::getGlobal().error(std::string("Failed to load plugin: ") + 
                                 result.pluginName + " - " + result.errorMessage);
    }
}

// ============================================================================
// IPlugin Implementation
// =============================================================================

IPlugin::~IPlugin() = default;

// ============================================================================
// ISensorPlugin Implementation
// =============================================================================

ISensorPlugin::~ISensorPlugin() = default;

// ============================================================================
// INeuralNetworkPlugin Implementation
// =============================================================================

INeuralNetworkPlugin::~INeuralNetworkPlugin() = default;

// ============================================================================
// IWorldPlugin Implementation
// =============================================================================

IWorldPlugin::~IWorldPlugin() = default;

} // namespace nlm