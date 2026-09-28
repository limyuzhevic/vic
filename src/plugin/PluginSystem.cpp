// PluginSystem.cpp - Implementation of the plugin system
#include "PluginSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <dirent.h>
#include <cstring>
#include <algorithm>

namespace nlm {

PluginSystem::PluginSystem() {
}

PluginSystem::~PluginSystem() {
    clear();
}

bool PluginSystem::loadPlugin(const std::string& pluginPath) {
    // Check if plugin is already loaded
    std::string pluginName = pluginPath;
    size_t lastSlash = pluginPath.find_last_of('/');
    if (lastSlash != std::string::npos) {
        pluginName = pluginPath.substr(lastSlash + 1);
    }
    
    if (isPluginLoaded(pluginName)) {
        NLM_LOG_WARNING("Plugin already loaded: " + pluginName);
        return true;
    }
    
    // Open plugin library
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        NLM_LOG_ERROR("Failed to load plugin " + pluginPath + ": " + getPluginError());
        return false;
    }
    
    // Try to create plugin instance using factory function
    using CreatePluginFunc = std::shared_ptr<Plugin>(*)();
    auto createPlugin = reinterpret_cast<CreatePluginFunc>(
        dlsym(handle, "createPlugin"));
    
    if (!createPlugin) {
        NLM_LOG_ERROR("Plugin " + pluginName + " does not have createPlugin factory function");
        dlclose(handle);
        return false;
    }
    
    // Create plugin instance
    auto plugin = createPlugin();
    if (!plugin) {
        NLM_LOG_ERROR("Failed to create plugin instance for " + pluginName);
        dlclose(handle);
        return false;
    }
    
    // Initialize plugin
    if (!plugin->initialize()) {
        NLM_LOG_ERROR("Failed to initialize plugin " + pluginName);
        dlclose(handle);
        return false;
    }
    
    // Store plugin
    PluginHandle pluginHandle;
    pluginHandle.handle = handle;
    pluginHandle.plugin = plugin;
    pluginHandle.path = pluginPath;
    
    plugins_[pluginName] = pluginHandle;
    
    NLM_LOG_INFO("Loaded plugin: " + pluginName + " (v" + plugin->getVersion() + ") - " + plugin->getDescription());
    return true;
}

bool PluginSystem::unloadPlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        NLM_LOG_WARNING("Plugin not found: " + pluginName);
        return false;
    }
    
    // Shutdown plugin
    if (it->second.plugin) {
        it->second.plugin->shutdown();
    }
    
    // Close library
    if (it->second.handle) {
        dlclose(it->second.handle);
    }
    
    plugins_.erase(it);
    NLM_LOG_INFO("Unloaded plugin: " + pluginName);
    return true;
}

std::vector<std::string> PluginSystem::getLoadedPlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& pair : plugins_) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

std::shared_ptr<Plugin> PluginSystem::getPlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return nullptr;
    }
    return it->second.plugin;
}

bool PluginSystem::executeCommand(const std::string& pluginName, const std::string& command,
                                 const std::string& args) {
    auto plugin = getPlugin(pluginName);
    if (!plugin) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    // This is a simple command interface - could be extended with more sophisticated command parsing
    if (command == "status") {
        NLM_LOG_INFO(pluginName + " status: " + plugin->getName() + 
                    " v" + plugin->getVersion() + " - " + plugin->getDescription());
        return true;
    }
    
    NLM_LOG_WARNING("Unknown command for plugin " + pluginName + ": " + command);
    return false;
}

bool PluginSystem::isPluginLoaded(const std::string& pluginName) const {
    return plugins_.find(pluginName) != plugins_.end();
}

void PluginSystem::clear() {
    for (auto& pair : plugins_) {
        if (pair.second.plugin) {
            pair.second.plugin->shutdown();
        }
        if (pair.second.handle) {
            dlclose(pair.second.handle);
        }
    }
    plugins_.clear();
    NLM_LOG_INFO("Plugin system cleared");
}

std::string PluginSystem::getSystemInfo() const {
    std::stringstream ss;
    ss << "Plugin System Info:"
       << "\nLoaded plugins: " << plugins_.size()
       << "\nFactory registry: " << factoryRegistry_.size();
    
    ss << "\n\nLoaded plugins:";
    for (const auto& pair : plugins_) {
        ss << "\n  - " << pair.first << " (" << pair.second.plugin->getName() << ")";
    }
    
    return ss.str();
}

std::string PluginSystem::getPluginError() {
    return std::string(dlerror());
}

// PluginManager implementation
PluginManager& PluginManager::getInstance() {
    static PluginManager instance;
    return instance;
}

bool PluginManager::initialize() {
    if (initialized_) {
        return true;
    }
    
    pluginSystem_ = std::make_unique<PluginSystem>();
    initialized_ = true;
    
    NLM_LOG_INFO("Plugin manager initialized");
    return true;
}

void PluginManager::shutdown() {
    if (!initialized_) {
        return;
    }
    
    pluginSystem_->clear();
    pluginSystem_.reset();
    initialized_ = false;
    
    NLM_LOG_INFO("Plugin manager shutdown");
}

bool PluginManager::loadPluginsFromDirectory(const std::string& directoryPath) {
    if (!initialized_) {
        NLM_LOG_ERROR("Plugin manager not initialized");
        return false;
    }
    
    DIR* dir = opendir(directoryPath.c_str());
    if (!dir) {
        NLM_LOG_ERROR("Failed to open plugin directory: " + directoryPath);
        return false;
    }
    
    bool success = true;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) {
            std::string filename = entry->d_name;
            if (filename.find(".so") != std::string::npos || 
                filename.find(".dylib") != std::string::npos ||
                filename.find(".dll") != std::string::npos) {
                
                std::string fullPath = directoryPath + "/" + filename;
                if (!pluginSystem_->loadPlugin(fullPath)) {
                    NLM_LOG_ERROR("Failed to load plugin: " + filename);
                    success = false;
                }
            }
        }
    }
    
    closedir(dir);
    return success;
}

PluginSystem& PluginManager::getPluginSystem() {
    if (!initialized_) {
        initialize();
    }
    return *pluginSystem_;
}

bool PluginManager::enablePlugin(const std::string& pluginName) {
    auto plugin = pluginSystem_->getPlugin(pluginName);
    if (!plugin) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    // Implementation would enable plugin
    NLM_LOG_INFO("Enabled plugin: " + pluginName);
    return true;
}

bool PluginManager::disablePlugin(const std::string& pluginName) {
    auto plugin = pluginSystem_->getPlugin(pluginName);
    if (!plugin) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    // Implementation would disable plugin
    NLM_LOG_INFO("Disabled plugin: " + pluginName);
    return true;
}

bool PluginManager::reloadPlugin(const std::string& pluginName) {
    if (!pluginSystem_->isPluginLoaded(pluginName)) {
        NLM_LOG_ERROR("Plugin not loaded: " + pluginName);
        return false;
    }
    
    // Implementation would reload plugin
    NLM_LOG_INFO("Reloaded plugin: " + pluginName);
    return true;
}

std::vector<std::string> PluginManager::discoverPlugins(const std::string& directoryPath) {
    std::vector<std::string> plugins;
    
    DIR* dir = opendir(directoryPath.c_str());
    if (!dir) {
        return plugins;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) {
            std::string filename = entry->d_name;
            if (filename.find(".so") != std::string::npos || 
                filename.find(".dylib") != std::string::npos ||
                filename.find(".dll") != std::string::npos) {
                plugins.push_back(filename);
            }
        }
    }
    
    closedir(dir);
    return plugins;
}

std::string PluginManager::getPluginInfo(const std::string& pluginName) {
    auto plugin = pluginSystem_->getPlugin(pluginName);
    if (!plugin) {
        return "Plugin not found: " + pluginName;
    }
    
    std::stringstream ss;
    ss << "Plugin: " << plugin->getName()
       << "\nVersion: " << plugin->getVersion()
       << "\nDescription: " << plugin->getDescription()
       << "\nStatus: Enabled";
    
    return ss.str();
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& pluginName) {
    // This would be implemented based on plugin metadata
    // For now, return empty vector
    return std::vector<std::string>();
}