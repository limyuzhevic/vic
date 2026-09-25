#include "PluginRegistry.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>

namespace nlm {

namespace {
    std::string generatePluginId() {
        return "plugin_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    }
    
    std::string extractFileName(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        return (pos != std::string::npos) ? path.substr(pos + 1) : path;
    }
    
    std::string extractFileExtension(const std::string& path) {
        size_t pos = path.find_last_of('.');
        return (pos != std::string::npos) ? path.substr(pos + 1) : "";
    }
    
    bool isPluginFile(const std::string& filePath) {
        std::string ext = extractFileExtension(filePath);
        return ext == "so" || ext == "dll" || ext == "dylib" || ext == "plugin";
    }
}

bool PluginRegistry::registerPlugin(const PluginMetadata& metadata) {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    if (!metadata.validate()) {
        NLM_LOG_ERROR("Plugin metadata validation failed for plugin: " + metadata.name);
        return false;
    }
    
    // Check if plugin already registered
    if (registeredPlugins_.find(metadata.name) != registeredPlugins_.end()) {
        NLM_LOG_WARNING("Plugin already registered: " + metadata.name);
        return false;
    }
    
    // Validate dependencies
    if (!validatePluginDependencies(metadata)) {
        NLM_LOG_ERROR("Plugin dependency validation failed for plugin: " + metadata.name);
        return false;
    }
    
    registeredPlugins_[metadata.name] = metadata;
    NLM_LOG_INFO("Plugin registered: " + metadata.name + " (" + metadata.description + ")");
    
    return true;
}

bool PluginRegistry::unregisterPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    auto it = registeredPlugins_.find(name);
    if (it == registeredPlugins_.end()) {
        NLM_LOG_WARNING("Plugin not found: " + name);
        return false;
    }
    
    // If plugin is currently loaded, unload it first
    if (activeInstances_.find(name) != activeInstances_.end()) {
        NLM_LOG_INFO("Unloading plugin before unregistering: " + name);
        activeInstances_.erase(name);
    }
    
    registeredPlugins_.erase(it);
    NLM_LOG_INFO("Plugin unregistered: " + name);
    
    return true;
}

bool PluginRegistry::unregisterPluginByPath(const std::string& filePath) {
    auto it = findPluginByPath(filePath);
    if (!it) return false;
    
    return unregisterPlugin(it->name);
}

std::vector<PluginMetadata> PluginRegistry::getAllPlugins() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    std::vector<PluginMetadata> plugins;
    plugins.reserve(registeredPlugins_.size());
    
    for (const auto& pair : registeredPlugins_) {
        plugins.push_back(pair.second);
    }
    
    return plugins;
}

std::vector<PluginMetadata> PluginRegistry::getPluginsByCategory(PluginCategory category) const {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    std::vector<PluginMetadata> plugins;
    
    for (const auto& pair : registeredPlugins_) {
        if (pair.second.category == category && pair.second.enabled) {
            plugins.push_back(pair.second);
        }
    }
    
    return plugins;
}

std::vector<PluginMetadata> PluginRegistry::getPluginsByPriority(PluginPriority priority) const {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    std::vector<PluginMetadata> plugins;
    
    for (const auto& pair : registeredPlugins_) {
        if (pair.second.priority == priority && pair.second.enabled) {
            plugins.push_back(pair.second);
        }
    }
    
    return plugins;
}

std::vector<PluginMetadata> PluginRegistry::getEnabledPlugins() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    std::vector<PluginMetadata> plugins;
    
    for (const auto& pair : registeredPlugins_) {
        if (pair.second.enabled) {
            plugins.push_back(pair.second);
        }
    }
    
    return plugins;
}

std::vector<PluginMetadata> PluginRegistry::getDisabledPlugins() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    std::vector<PluginMetadata> plugins;
    
    for (const auto& pair : registeredPlugins_) {
        if (!pair.second.enabled) {
            plugins.push_back(pair.second);
        }
    }
    
    return plugins;
}

bool PluginRegistry::loadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    auto metadataIt = findPluginByName(name);
    if (!metadataIt) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    if (activeInstances_.find(name) != activeInstances_.end()) {
        NLM_LOG_WARNING("Plugin already loaded: " + name);
        return true;  // Already loaded is not an error
    }
    
    try {
        auto instance = metadataIt->factory();
        auto* rawPtr = instance.get();
        
        if (initializePlugin(rawPtr, metadataIt->second)) {
            activeInstances_[name] = std::move(instance);
            NLM_LOG_INFO("Plugin loaded successfully: " + name);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to initialize plugin: " + name);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading plugin " + name + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginRegistry::loadPluginByPath(const std::string& filePath) {
    // This would normally involve dynamic loading of shared libraries
    // For now, we'll check if the plugin is already registered
    auto metadataIt = findPluginByPath(filePath);
    if (!metadataIt) {
        NLM_LOG_ERROR("No plugin registered for file: " + filePath);
        return false;
    }
    
    return loadPlugin(metadataIt->name);
}

bool PluginRegistry::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    auto it = activeInstances_.find(name);
    if (it == activeInstances_.end()) {
        NLM_LOG_WARNING("Plugin not currently loaded: " + name);
        return false;
    }
    
    // Call shutdown callbacks before unloading
    shutdownPlugin(it->second.get());
    
    // Remove from active instances
    activeInstances_.erase(it);
    
    NLM_LOG_INFO("Plugin unloaded: " + name);
    return true;
}

bool PluginRegistry::reloadPlugin(const std::string& name) {
    if (!unloadPlugin(name)) return false;
    return loadPlugin(name);
}

bool PluginRegistry::enablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    auto it = registeredPlugins_.find(name);
    if (it == registeredPlugins_.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    if (it->second.enabled) {
        NLM_LOG_WARNING("Plugin already enabled: " + name);
        return true;
    }
    
    it->second.enabled = true;
    NLM_LOG_INFO("Plugin enabled: " + name);
    
    return true;
}

bool PluginRegistry::disablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    auto it = registeredPlugins_.find(name);
    if (it == registeredPlugins_.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    if (!it->second.enabled) {
        NLM_LOG_WARNING("Plugin already disabled: " + name);
        return true;
    }
    
    it->second.enabled = false;
    NLM_LOG_INFO("Plugin disabled: " + name);
    
    return true;
}

PluginRegistry::PluginMetadata* PluginRegistry::findPluginByName(const std::string& name) {
    auto it = registeredPlugins_.find(name);
    return it != registeredPlugins_.end() ? &it->second : nullptr;
}

const PluginRegistry::PluginMetadata* PluginRegistry::findPluginByName(const std::string& name) const {
    auto it = registeredPlugins_.find(name);
    return it != registeredPlugins_.end() ? &it->second : nullptr;
}

PluginRegistry::PluginMetadata* PluginRegistry::findPluginByPath(const std::string& filePath) {
    for (auto& pair : registeredPlugins_) {
        if (pair.second.filePath == filePath) {
            return &pair.second;
        }
    }
    return nullptr;
}

const PluginRegistry::PluginMetadata* PluginRegistry::findPluginByPath(const std::string& filePath) const {
    for (const auto& pair : registeredPlugins_) {
        if (pair.second.filePath == filePath) {
            return &pair.second;
        }
    }
    return nullptr;
}

bool PluginRegistry::initializePlugin(IPlugin* plugin, const PluginMetadata& metadata) {
    if (!plugin) {
        NLM_LOG_ERROR("Cannot initialize null plugin");
        return false;
    }
    
    try {
        // Call init callbacks
        for (const auto& callback : initCallbacks_) {
            callback(plugin);
        }
        
        // Initialize the plugin
        std::any config;
        if (!initializePluginConfiguration(plugin, metadata, config)) {
            NLM_LOG_ERROR("Failed to initialize plugin configuration for: " + metadata.name);
            return false;
        }
        
        if (!plugin->initialize(config)) {
            NLM_LOG_ERROR("Plugin initialization failed: " + metadata.name);
            return false;
        }
        
        plugin->initialized_ = true;
        
        NLM_LOG_INFO("Plugin initialized: " + metadata.name);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing plugin " + metadata.name + ": " + std::string(e.what()));
        return false;
    }
}

void PluginRegistry::shutdownPlugin(IPlugin* plugin) {
    if (!plugin) return;
    
    try {
        if (plugin->initialized_) {
            // Call shutdown callbacks
            for (const auto& callback : shutdownCallbacks_) {
                callback(plugin);
            }
            
            plugin->shutdown();
            plugin->initialized_ = false;
            
            NLM_LOG_INFO("Plugin shutdown: " + plugin->getName());
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down plugin " + std::string(e.what()));
    }
}

bool PluginRegistry::initializePluginConfiguration(IPlugin* plugin, const PluginMetadata& metadata, std::any& config) {
    // Default implementation - subclasses can override this
    // For now, just set an empty config
    config = std::any();
    return true;
}

bool PluginRegistry::validatePluginDependencies(const PluginMetadata& metadata) const {
    for (const auto& dependency : metadata.dependencies) {
        if (registeredPlugins_.find(dependency) == registeredPlugins_.end()) {
            NLM_LOG_ERROR("Plugin dependency not found: " + dependency + " (required by " + metadata.name + ")");
            return false;
        }
        
        const auto& depPlugin = registeredPlugins_.at(dependency);
        if (!depPlugin.enabled) {
            NLM_LOG_ERROR("Plugin dependency not enabled: " + dependency + " (required by " + metadata.name + ")");
            return false;
        }
    }
    
    return true;
}

bool PluginRegistry::initializePluginSystem() {
    if (initialized_) {
        NLM_LOG_WARNING("Plugin system already initialized");
        return true;
    }
    
    try {
        // Load all enabled plugins
        for (const auto& pair : registeredPlugins_) {
            if (pair.second.enabled) {
                if (!loadPlugin(pair.first)) {
                    NLM_LOG_ERROR("Failed to load plugin during system initialization: " + pair.first);
                    return false;
                }
            }
        }
        
        initialized_ = true;
        startTime_ = std::chrono::steady_clock::now();
        
        NLM_LOG_INFO("Plugin system initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing plugin system: " + std::string(e.what()));
        return false;
    }
}

void PluginRegistry::shutdownPluginSystem() {
    if (!initialized_) {
        return;
    }
    
    try {
        // Shutdown all loaded plugins
        for (auto it = activeInstances_.begin(); it != activeInstances_.end(); ) {
            shutdownPlugin(it->second.get());
            it = activeInstances_.erase(it);
        }
        
        initialized_ = false;
        
        NLM_LOG_INFO("Plugin system shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down plugin system: " + std::string(e.what()));
    }
}

bool PluginRegistry::isPluginSystemInitialized() const {
    return initialized_;
}

std::vector<std::string> PluginRegistry::scanPluginDirectories(const std::vector<std::string>& directories) {
    std::vector<std::string> pluginFiles;
    
    for (const auto& directory : directories) {
        if (!std::filesystem::exists(directory)) {
            NLM_LOG_WARNING("Plugin directory does not exist: " + directory);
            continue;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && isPluginFile(entry.path().string())) {
                pluginFiles.push_back(entry.path().string());
            }
        }
    }
    
    return pluginFiles;
}

bool PluginRegistry::loadPluginsFromDirectory(const std::string& directory) {
    std::vector<std::string> pluginFiles = scanPluginDirectories({directory});
    
    bool success = true;
    for (const auto& filePath : pluginFiles) {
        if (!loadPluginByPath(filePath)) {
            NLM_LOG_ERROR("Failed to load plugin from: " + filePath);
            success = false;
        }
    }
    
    return success;
}

std::string PluginRegistry::getPluginReport() const {
    std::ostringstream oss;
    
    oss << "=== NLM Plugin System Report ===" << std::endl;
    oss << "Version: 1.0" << std::endl;
    oss << "Initialized: " << (initialized_ ? "Yes" : "No") << std::endl;
    oss << "Total Plugins Registered: " << registeredPlugins_.size() << std::endl;
    oss << "Enabled Plugins: " << std::count_if(registeredPlugins_.begin(), registeredPlugins_.end(),
                                     [](const auto& pair) { return pair.second.enabled; }) << std::endl;
    oss << "Active Plugin Instances: " << activeInstances_.size() << std::endl;
    
    oss << "\nPlugins by Category:" << std::endl;
    for (const auto& pair : registeredPlugins_) {
        oss << "  " << static_cast<int>(pair.second.category) << ": " << pair.first << " (" 
            << (pair.second.enabled ? "enabled" : "disabled") << ")" << std::endl;
    }
    
    oss << "\nPlugin Status:" << std::endl;
    for (const auto& pair : activeInstances_) {
        oss << "  " << pair.first << ": " << pair.second->getStatus() << std::endl;
    }
    
    return oss.str();
}

PluginRegistry::PluginStats PluginRegistry::getStatistics() const {
    PluginStats stats;
    
    stats.totalPlugins = registeredPlugins_.size();
    stats.enabledPlugins = std::count_if(registeredPlugins_.begin(), registeredPlugins_.end(),
                                       [](const auto& pair) { return pair.second.enabled; });
    stats.disabledPlugins = stats.totalPlugins - stats.enabledPlugins;
    
    for (const auto& pair : registeredPlugins_) {
        stats.pluginsByCategory[pair.second.category]++;
    }
    
    for (const auto& pair : activeInstances_) {
        if (pair.second->initialized_) {
            auto loadTime = std::chrono::duration<double>(
                std::chrono::steady_clock::now() - pair.second->creationTime_).count();
            stats.totalLoadTime += loadTime;
        }
    }
    
    if (activeInstances_.size() > 0) {
        stats.totalLoadTime /= activeInstances_.size();
    }
    
    return stats;
}

void PluginRegistry::registerInitCallback(const PluginInitCallback& callback) {
    initCallbacks_.push_back(callback);
}

void PluginRegistry::registerShutdownCallback(const PluginShutdownCallback& callback) {
    shutdownCallbacks_.push_back(callback);
}

void PluginRegistry::registerErrorCallback(const PluginErrorCallback& callback) {
    errorCallbacks_.push_back(callback);
}

bool PluginRegistry::validatePluginConfiguration(const std::string& name, const std::any& config) {
    auto metadataIt = findPluginByName(name);
    if (!metadataIt) return false;
    
    return validatePluginConfiguration(*metadataIt, config);
}

bool PluginRegistry::validatePluginConfiguration(const PluginMetadata& metadata, const std::any& config) const {
    // Default implementation - can be overridden by subclasses
    return true;
}

bool PluginRegistry::exportPlugins(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for plugin export: " + filePath);
        return false;
    }
    
    // Simple export format - JSON-like
    file << "{\"plugins\": [" << std::endl;
    
    bool first = true;
    for (const auto& pair : registeredPlugins_) {
        if (!first) file << "," << std::endl;
        first = false;
        
        file << "  {\"name\": \"" << pair.second.name << "\", \"description\": \"" 
             << pair.second.description << "\", \"category\": " << static_cast<int>(pair.second.category)
             << ", \"enabled\": " << (pair.second.enabled ? "true" : "false") << "}";
    }
    
    file << std::endl << "]" << std::endl;
    file.close();
    
    NLM_LOG_INFO("Plugins exported to: " + filePath);
    return true;
}

bool PluginRegistry::importPlugins(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for plugin import: " + filePath);
        return false;
    }
    
    // Simple import format - JSON-like
    std::string line;
    bool inPluginsArray = false;
    bool inPluginObject = false;
    
    while (std::getline(file, line)) {
        // Very basic parsing - in production, use a proper JSON parser
        if (line.find("{\"plugins\": [") != std::string::npos) {
            inPluginsArray = true;
            continue;
        }
        
        if (inPluginsArray && line.find("}") != std::string::npos) {
            inPluginObject = false;
            continue;
        }
        
        if (inPluginsArray && !line.empty() && line.find(",") == std::string::npos) {
            // Parse plugin name
            size_t namePos = line.find("\"name\": \"");
            if (namePos != std::string::npos) {
                size_t nameStart = namePos + 9;
                size_t nameEnd = line.find("\",", nameStart);
                if (nameEnd != std::string::npos) {
                    std::string name = line.substr(nameStart, nameEnd - nameStart);
                    
                    // Load the plugin if it's enabled
                    auto metadataIt = findPluginByName(name);
                    if (metadataIt && metadataIt->second.enabled) {
                        loadPlugin(name);
                    }
                }
            }
        }
    }
    
    file.close();
    
    NLM_LOG_INFO("Plugins imported from: " + filePath);
    return true;
}

bool PluginRegistry::backupPlugins(const std::string& backupPath) {
    std::string timestamp = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime_).count());
    
    std::string backupFile = backupPath + "/nlm_plugins_" + timestamp + ".backup";
    
    return exportPlugins(backupFile);
}

} // namespace nlm
