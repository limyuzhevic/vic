// ============================================================================
// Plugin Manager Implementation
// ============================================================================

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

namespace nlm {

PluginManager::PluginManager() : pluginsMutex_() {}

PluginManager::~PluginManager() {
    // Cleanup all plugins
    for (auto& pair : loadedPlugins_) {
        if (pair.second.plugin) {
            try {
                pair.second.plugin->shutdown();
            } catch (...) {
                // Ignore exceptions during shutdown
            }
        }
    }
    loadedPlugins_.clear();
}

bool PluginManager::loadPlugin(const std::string& filepath) {
    try {
        // Check if plugin already loaded
        std::string pluginName = extractPluginName(filepath);
        if (isPluginLoaded(pluginName)) {
            return false;
        }
        
        // Load plugin (simplified - would use dynamic loading in real implementation)
        std::shared_ptr<IPlugin> plugin = createPluginInstance(filepath);
        
        if (!plugin) {
            return false;
        }
        
        // Initialize plugin
        Config defaultConfig;
        if (!plugin->initialize(defaultConfig)) {
            return false;
        }
        
        // Add to loaded plugins
        LoadedPlugin loadedPlugin;
        loadedPlugin.name = pluginName;
        loadedPlugin.plugin = plugin;
        loadedPlugin.filepath = filepath;
        loadedPlugin.isEnabled = true;
        loadedPlugin.loadedAt = std::chrono::system_clock::now();
        
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        loadedPlugins_[pluginName] = loadedPlugin;
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool PluginManager::unloadPlugin(const std::string& name) {
    try {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        
        auto it = loadedPlugins_.find(name);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        // Shutdown plugin
        if (it->second.plugin) {
            it->second.plugin->shutdown();
        }
        
        loadedPlugins_.erase(it);
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    try {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        
        std::vector<std::string> names;
        names.reserve(loadedPlugins_.size());
        
        for (const auto& pair : loadedPlugins_) {
            if (pair.second.isEnabled) {
                names.push_back(pair.first);
            }
        }
        
        return names;
        
    } catch (const std::exception& e) {
        return std::vector<std::string>();
    }
}

bool PluginManager::isPluginLoaded(const std::string& name) const {
    try {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        
        return loadedPlugins_.find(name) != loadedPlugins_.end();
        
    } catch (const std::exception& e) {
        return false;
    }
}

PluginManager::PluginInfo PluginManager::getPluginInfo(const std::string& name) const {
    PluginInfo info;
    info.isEnabled = false;
    info.isValid = false;
    
    try {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        
        auto it = loadedPlugins_.find(name);
        if (it == loadedPlugins_.end()) {
            return info;
        }
        
        auto& loaded = it->second;
        
        info.name = loaded.name;
        info.version = "1.0.0"; // Would get from plugin
        info.description = "Plugin: " + loaded.name;
        info.author = "Unknown";
        info.filepath = loaded.filepath;
        info.isEnabled = loaded.isEnabled;
        info.isValid = true;
        
        // Get dependencies
        info.dependencies = loaded.plugin->getDependencies();
        
        return info;
        
    } catch (const std::exception& e) {
        return info;
    }
}

bool PluginManager::callPluginFunction(const std::string& pluginName, const std::string& functionName,
                                      const std::vector<std::any>& args, std::any& result) {
    try {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        
        auto it = loadedPlugins_.find(pluginName);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        if (!it->second.isEnabled) {
            return false;
        }
        
        return it->second.plugin->callFunction(functionName, args, result);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool PluginManager::setPluginEnabled(const std::string& name, bool enabled) {
    try {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        
        auto it = loadedPlugins_.find(name);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        it->second.isEnabled = enabled;
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> PluginManager::getPluginFunctions(const std::string& name) const {
    try {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        
        auto it = loadedPlugins_.find(name);
        if (it == loadedPlugins_.end()) {
            return std::vector<std::string>();
        }
        
        return it->second.plugin->getFunctionNames();
        
    } catch (const std::exception& e) {
        return std::vector<std::string>();
    }
}

bool PluginManager::savePluginConfig(const std::string& name, const std::string& config) const {
    try {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        
        auto it = loadedPlugins_.find(name);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        // Save config (would persist to file in real implementation)
        // For now, just store in memory
        it->second.config = config;
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool PluginManager::loadPluginConfig(const std::string& name, std::string& config) const {
    try {
        std::lock_guard<std::mutex> lock(pluginsMutex_);
        
        auto it = loadedPlugins_.find(name);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        config = it->second.config;
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

// Helper functions for plugin management
std::string PluginManager::extractPluginName(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos) {
        return filepath.substr(pos + 1);
    }
    return filepath;
}

std::shared_ptr<PluginManager::IPlugin> PluginManager::createPluginInstance(const std::string& filepath) {
    // In a real implementation, this would use dynamic loading
    // For now, return nullptr
    return nullptr;
}

} // namespace nlm
