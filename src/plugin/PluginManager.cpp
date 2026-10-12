#include "PluginManager.hpp"
#include "PluginConfig.hpp"
#include "PluginSecurity.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

struct PluginManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<Plugin>> plugins;
    std::unordered_map<std::string, PluginStatus> pluginStatuses;
    std::unordered_map<std::string, PluginMetadata> pluginMetadata;
    std::unordered_map<std::string, std::shared_ptr<class PluginConfig>> pluginConfigs;
    std::unordered_map<std::string, std::vector<std::string>> pluginExtensionPoints;
    std::unordered_map<std::string, std::vector<std::string>> pluginProvidedServices;
    
    std::mutex mutex;
    PluginLoader loader;
    std::shared_ptr<PluginSecurity> security;
    
    // Extension point registry
    struct ExtensionPoint {
        PluginType type;
        std::function<bool(std::shared_ptr<Plugin>)> validator;
        std::vector<std::shared_ptr<Plugin>> providers;
    };
    std::unordered_map<std::string, ExtensionPoint> extensionPoints;
    
    // Plugin communication
    std::function<void(const std::string&, const std::string&, const std::string&)> messageHandler;
    
    // Statistics
    std::chrono::steady_clock::time_point lastLoadTime;
    float totalLoadTime = 0.0f;
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->security = std::make_shared<PluginSecurity>();
}

PluginManager::~PluginManager() {
    shutdown();
}

bool PluginManager::initialize(const PluginSettings& settings) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (initialized_) {
        return false; // Already initialized
    }
    
    settings_ = settings;
    
    try {
        // Create plugin directory if it doesn't exist
        if (!settings_.pluginDirectory.empty()) {
            std::filesystem::create_directories(settings_.pluginDirectory);
        }
        
        // Initialize security system
        if (settings_.sandboxPlugins) {
            pImpl->security->initialize();
        }
        
        // Auto-load plugins if enabled
        if (settings_.autoLoad) {
            enumeratePlugins();
            if (settings_.enableAllPlugins) {
                for (const auto& name : getAvailablePlugins()) {
                    loadPlugin(name);
                }
            }
        }
        
        initialized_ = true;
        
        if (settings_.logPluginActivity) {
            NLM_LOG_INFO("PluginManager initialized successfully");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> errorLock(errorsMutex_);
        recentErrors_.push_back("Failed to initialize PluginManager: " + std::string(e.what()));
        return false;
    }
}

PluginLoader::LoadResult PluginManager::loadPlugin(const std::string& filepath, PluginType expectedType) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    PluginLoader::LoadResult result;
    
    try {
        // Security check
        if (settings_.sandboxPlugins) {
            if (!pImpl->security->isPathSafe(filepath)) {
                result.errorMessage = "Security check failed for plugin: " + filepath;
                return result;
            }
        }
        
        // Load plugin using loader
        result = pImpl->loader.loadPlugin(filepath, expectedType);
        if (!result.success) {
            std::lock_guard<std::mutex> errorLock(errorsMutex_);
            recentErrors_.push_back("Failed to load plugin " + filepath + ": " + result.errorMessage);
            return result;
        }
        
        // Validate plugin
        if (settings_.validatePlugins) {
            if (!validatePlugin(result.plugin)) {
                pImpl->loader.unloadPlugin(result.plugin->getInstanceId());
                result.success = false;
                result.errorMessage = "Plugin validation failed";
                std::lock_guard<std::mutex> errorLock(errorsMutex_);
                recentErrors_.push_back("Plugin validation failed for " + filepath);
                return result;
            }
        }
        
        // Store plugin
        std::string instanceId = result.plugin->getInstanceId();
        pImpl->plugins[instanceId] = result.plugin;
        pImpl->pluginMetadata[instanceId] = result.plugin->getMetadata();
        pImpl->pluginStatuses[instanceId] = PluginStatus::Loaded;
        
        // Initialize plugin
        auto config = std::make_shared<PluginConfig>();
        pImpl->pluginConfigs[instanceId] = config;
        
        bool initSuccess = result.plugin->initialize(shared_from_this(), config);
        if (!initSuccess) {
            // Clean up on initialization failure
            pImpl->plugins.erase(instanceId);
            pImpl->pluginMetadata.erase(instanceId);
            pImpl->pluginStatuses.erase(instanceId);
            pImpl->pluginConfigs.erase(instanceId);
            
            pImpl->loader.unloadPlugin(instanceId);
            result.success = false;
            result.errorMessage = "Plugin initialization failed";
            std::lock_guard<std::mutex> errorLock(errorsMutex_);
            recentErrors_.push_back("Plugin initialization failed for " + filepath);
            return result;
        }
        
        // Update statistics
        totalLoadCount_++;
        if (pImpl->lastLoadTime.time_since_epoch().count() > 0) {
            auto current = std::chrono::steady_clock::now();
            float loadTime = std::chrono::duration<float>(current - pImpl->lastLoadTime).count();
            pImpl->totalLoadTime += loadTime;
            averageLoadTime_ = pImpl->totalLoadTime / totalLoadCount_;
        }
        pImpl->lastLoadTime = std::chrono::steady_clock::now();
        
        // Register extension points
        auto extensionPoints = result.plugin->getExtensionPoints();
        pImpl->pluginExtensionPoints[instanceId] = extensionPoints;
        
        // Register provided services
        auto services = result.plugin->getProvidedServices();
        pImpl->pluginProvidedServices[instanceId] = services;
        
        if (settings_.logPluginActivity) {
            NLM_LOG_INFO("Successfully loaded plugin: " + result.plugin->getMetadata().name + 
                        " (version " + std::to_string(result.plugin->getMetadata().version.major) + "." +
                        std::to_string(result.plugin->getMetadata().version.minor) + "." +
                        std::to_string(result.plugin->getMetadata().version.patch) + ")");
        }
        
        return result;
        
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> errorLock(errorsMutex_);
        recentErrors_.push_back("Exception during plugin load " + filepath + ": " + std::string(e.what()));
        return result;
    }
}

PluginLoader::LoadResult PluginManager::loadPlugin(const std::string& name, const PluginVersion& version) {
    // Try to create from factory first
    PluginLoader::LoadResult result = pImpl->loader.createPlugin(name);
    if (result.success) {
        // Set version if provided
        if (version.major > 0) {
            result.plugin->getMetadata().version = version;
        }
        
        // Use file path as instance ID for factory-created plugins
        std::string instanceId = result.plugin->getInstanceId();
        
        // Store plugin
        pImpl->plugins[instanceId] = result.plugin;
        pImpl->pluginMetadata[instanceId] = result.plugin->getMetadata();
        pImpl->pluginStatuses[instanceId] = PluginStatus::Loaded;
        
        // Initialize plugin
        auto config = std::make_shared<PluginConfig>();
        pImpl->pluginConfigs[instanceId] = config;
        
        bool initSuccess = result.plugin->initialize(shared_from_this(), config);
        if (!initSuccess) {
            pImpl->plugins.erase(instanceId);
            pImpl->pluginMetadata.erase(instanceId);
            pImpl->pluginStatuses.erase(instanceId);
            pImpl->pluginConfigs.erase(instanceId);
            
            result.success = false;
            result.errorMessage = "Plugin initialization failed";
            return result;
        }
        
        totalLoadCount_++;
        return result;
    }
    
    // If factory creation failed, try to find file in plugin directory
    if (!settings_.pluginDirectory.empty()) {
        std::string filepath = settings_.pluginDirectory + "/" + name + ".so";
        if (std::filesystem::exists(filepath)) {
            return loadPlugin(filepath);
        }
    }
    
    return result;
}

bool PluginManager::enablePlugin(const std::string& instanceId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(instanceId);
    if (it == pImpl->plugins.end()) {
        return false; // Plugin not found
    }
    
    try {
        it->second->enable();
        pImpl->pluginStatuses[instanceId] = PluginStatus::Enabled;
        
        if (settings_.logPluginActivity) {
            NLM_LOG_INFO("Plugin enabled: " + it->second->getMetadata().name);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> errorLock(errorsMutex_);
        recentErrors_.push_back("Failed to enable plugin " + instanceId + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginManager::disablePlugin(const std::string& instanceId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(instanceId);
    if (it == pImpl->plugins.end()) {
        return false; // Plugin not found
    }
    
    try {
        it->second->disable();
        pImpl->pluginStatuses[instanceId] = PluginStatus::Disabled;
        
        if (settings_.logPluginActivity) {
            NLM_LOG_INFO("Plugin disabled: " + it->second->getMetadata().name);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> errorLock(errorsMutex_);
        recentErrors_.push_back("Failed to disable plugin " + instanceId + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginManager::unloadPlugin(const std::string& instanceId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(instanceId);
    if (it == pImpl->plugins.end()) {
        return false; // Plugin not found
    }
    
    try {
        // Disable plugin first if enabled
        if (pImpl->pluginStatuses[instanceId] == PluginStatus::Enabled) {
            disablePlugin(instanceId);
        }
        
        // Cleanup plugin
        it->second->cleanup();
        
        // Remove from registry
        pImpl->plugins.erase(it);
        pImpl->pluginMetadata.erase(instanceId);
        pImpl->pluginStatuses.erase(instanceId);
        pImpl->pluginConfigs.erase(instanceId);
        pImpl->pluginExtensionPoints.erase(instanceId);
        pImpl->pluginProvidedServices.erase(instanceId);
        
        // Unload from loader
        pImpl->loader.unloadPlugin(instanceId);
        
        totalUnloadCount_++;
        
        if (settings_.logPluginActivity) {
            NLM_LOG_INFO("Plugin unloaded: " + instanceId);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> errorLock(errorsMutex_);
        recentErrors_.push_back("Failed to unload plugin " + instanceId + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginManager::restartPlugin(const std::string& instanceId) {
    if (!unloadPlugin(instanceId)) {
        return false;
    }
    
    // Find the original filepath or name
    // This is simplified - in practice you'd need to track the original source
    // For now, just return success
    return true;
}

std::vector<std::string> PluginManager::getAvailablePlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loader.enumeratePlugins();
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> loaded;
    for (const auto& pair : pImpl->pluginStatuses) {
        if (pair.second != PluginStatus::Inactive && pair.second != PluginStatus::Uninstalled) {
            loaded.push_back(pair.first);
        }
    }
    return loaded;
}

std::vector<std::string> PluginManager::getEnabledPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> enabled;
    for (const auto& pair : pImpl->pluginStatuses) {
        if (pair.second == PluginStatus::Enabled) {
            enabled.push_back(pair.first);
        }
    }
    return enabled;
}

std::shared_ptr<Plugin> PluginManager::getPlugin(const std::string& instanceId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(instanceId);
    if (it == pImpl->plugins.end()) {
        return nullptr;
    }
    
    return it->second;
}

PluginMetadata PluginManager::getPluginMetadata(const std::string& instanceId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginMetadata.find(instanceId);
    if (it == pImpl->pluginMetadata.end()) {
        return PluginMetadata(); // Return empty metadata
    }
    
    return it->second;
}

PluginStatus PluginManager::getPluginStatus(const std::string& instanceId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginStatuses.find(instanceId);
    if (it == pImpl->pluginStatuses.end()) {
        return PluginStatus::Inactive;
    }
    
    return it->second;
}

void PluginManager::configurePlugin(const std::string& instanceId, const std::shared_ptr<PluginConfig>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginConfigs.find(instanceId);
    if (it != pImpl->pluginConfigs.end()) {
        it->second = config;
    }
}

std::shared_ptr<PluginConfig> PluginManager::getPluginConfig(const std::string& instanceId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginConfigs.find(instanceId);
    if (it == pImpl->pluginConfigs.end()) {
        return nullptr;
    }
    
    return it->second;
}

void PluginManager::registerExtensionPoint(const std::string& name, PluginType type,
                                          const std::function<bool(std::shared_ptr<Plugin>)>& validator) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->extensionPoints[name] = {type, validator, {}};
}

bool PluginManager::requestExtensionPoint(const std::string& name, std::shared_ptr<Plugin> plugin) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->extensionPoints.find(name);
    if (it == pImpl->extensionPoints.end()) {
        return false; // Extension point not found
    }
    
    ExtensionPoint& ep = it->second;
    
    // Check if plugin is compatible
    if (ep.validator && !ep.validator(plugin)) {
        return false;
    }
    
    // Check if plugin already provides this extension point
    for (const auto& provider : ep.providers) {
        if (provider == plugin) {
            return false; // Already registered
        }
    }
    
    // Register plugin as provider
    ep.providers.push_back(plugin);
    
    return true;
}

void PluginManager::broadcastMessage(const std::string& message, const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getStatus() == PluginStatus::Enabled) {
            pair.second->onMessage(message, data);
        }
    }
}

void PluginManager::sendMessage(const std::string& target, const std::string& message, const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto targetIt = pImpl->plugins.find(target);
    if (targetIt != pImpl->plugins.end()) {
        targetIt->second->onMessage(message, data);
    }
}

void PluginManager::update(float dt) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getStatus() == PluginStatus::Enabled) {
            pair.second->update(dt);
        }
    }
}

void PluginManager::setPluginDirectory(const std::string& directory) {
    settings_.pluginDirectory = directory;
}

std::vector<std::string> PluginManager::getPluginsByType(PluginType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> pluginsByType;
    
    for (const auto& pair : pImpl->pluginMetadata) {
        if (pair.second.type == type) {
            pluginsByType.push_back(pair.first);
        }
    }
    
    return pluginsByType;
}

std::vector<std::string> PluginManager::getPluginsByExtensionPoint(const std::string& extensionPoint) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->extensionPoints.find(extensionPoint);
    if (it == pImpl->extensionPoints.end()) {
        return {};
    }
    
    std::vector<std::string> providers;
    for (const auto& provider : it->second.providers) {
        for (const auto& pair : pImpl->plugins) {
            if (pair.second == provider) {
                providers.push_back(pair.first);
                break;
            }
        }
    }
    
    return providers;
}

bool PluginManager::validatePlugin(std::shared_ptr<Plugin> plugin) {
    if (!plugin) {
        return false;
    }
    
    const auto& metadata = plugin->getMetadata();
    
    // Check required fields
    if (metadata.name.empty() || metadata.version.major == 0) {
        return false;
    }
    
    // Check version compatibility
    // (In a real implementation, this would check against minimum required version)
    
    // Check capabilities
    const auto& caps = metadata.capabilities;
    if (caps.requiresSandboxing && !settings_.sandboxPlugins) {
        return false;
    }
    
    // Validate using security module
    if (settings_.sandboxPlugins) {
        // Create a temporary path for validation
        return true; // Placeholder
    }
    
    return true;
}

std::vector<std::string> PluginManager::getRecentErrors() const {
    std::lock_guard<std::mutex> lock(errorsMutex_);
    return recentErrors_;
}

void PluginManager::clearRecentErrors() {
    std::lock_guard<std::mutex> lock(errorsMutex_);
    recentErrors_.clear();
}

void PluginManager::cleanup() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Unload all plugins
    std::vector<std::string> loadedPlugins = getLoadedPlugins();
    for (const auto& id : loadedPlugins) {
        unloadPlugin(id);
    }
    
    // Cleanup security module
    if (pImpl->security) {
        pImpl->security->cleanup();
    }
    
    initialized_ = false;
}

void PluginManager::shutdown() {
    cleanup();
    settings_ = PluginSettings(); // Reset to defaults
}

std::vector<std::string> PluginManager::enumeratePlugins() const {
    std::vector<std::string> plugins;
    
    if (settings_.pluginDirectory.empty()) {
        return plugins;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(settings_.pluginDirectory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();
                
                if (extension == ".so" || extension == ".dll" || extension == ".dylib") {
                    // Remove extension to get plugin name
                    size_t dotPos = filename.find_last_of('.');
                    if (dotPos != std::string::npos) {
                        std::string pluginName = filename.substr(0, dotPos);
                        plugins.push_back(pluginName);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> errorLock(errorsMutex_);
        recentErrors_.push_back("Error enumerating plugins: " + std::string(e.what()));
    }
    
    return plugins;
}

} // namespace nlm