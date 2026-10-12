#include "PluginManager.hpp"
#include "NLMPlugin.hpp"
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <sstream>

namespace nlm {

// Singleton implementation for PluginRegistry
PluginRegistry& PluginRegistry::getInstance() {
    static PluginRegistry instance;
    return instance;
}

bool PluginRegistry::discoverPlugins(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto pluginFiles = PluginLoader::getPluginFiles(directory);
    bool success = true;
    
    for (const auto& filepath : pluginFiles) {
        auto plugin = PluginLoader::loadPlugin(filepath);
        if (plugin) {
            if (registerPlugin(std::move(plugin))) {
                std::cout << "PluginRegistry: Discovered plugin: " << filepath << std::endl;
            } else {
                std::cout << "PluginRegistry: Failed to register plugin: " << filepath << std::endl;
                success = false;
            }
        } else {
            std::cout << "PluginRegistry: Failed to load plugin: " << filepath << std::endl;
            success = false;
        }
    }
    
    return success;
}

std::vector<std::string> PluginRegistry::getAvailablePlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::string> result;
    for (const auto& pair : plugins) {
        result.push_back(pair.first);
    }
    return result;
}

bool PluginRegistry::registerPlugin(std::unique_ptr<NLMPlugin> plugin) {
    if (!plugin) {
        std::cout << "PluginRegistry: Cannot register null plugin" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    
    const std::string& name = plugin->getMetadata().name;
    
    // Check if plugin already exists
    if (plugins.find(name) != plugins.end()) {
        std::cout << "PluginRegistry: Plugin already registered: " << name << std::endl;
        return false;
    }
    
    // Check dependencies
    if (!checkDependencies(plugin->getMetadata())) {
        std::cout << "PluginRegistry: Plugin has unresolved dependencies: " << name << std::endl;
        return false;
    }
    
    plugins[name] = std::move(plugin);
    std::cout << "PluginRegistry: Registered plugin: " << name << std::endl;
    return true;
}

bool PluginRegistry::unregisterPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        std::cout << "PluginRegistry: Plugin not found: " << pluginName << std::endl;
        return false;
    }
    
    plugins.erase(it);
    std::cout << "PluginRegistry: Unregistered plugin: " << pluginName << std::endl;
    return true;
}

std::unique_ptr<NLMPlugin> PluginRegistry::getPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        std::cout << "PluginRegistry: Plugin not found: " << pluginName << std::endl;
        return nullptr;
    }
    
    return std::unique_ptr<NLMPlugin>(it->second.get());
}

std::vector<std::unique_ptr<NLMPlugin>> PluginRegistry::getPluginsByCategory(PluginCategory category) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::unique_ptr<NLMPlugin>> result;
    for (const auto& pair : plugins) {
        if (pair.second->getMetadata().category == category) {
            result.push_back(std::unique_ptr<NLMPlugin>(pair.second.get()));
        }
    }
    return result;
}

std::vector<std::unique_ptr<NLMPlugin>> PluginRegistry::getPlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::unique_ptr<NLMPlugin>> result;
    for (const auto& pair : plugins) {
        result.push_back(std::unique_ptr<NLMPlugin>(pair.second.get()));
    }
    return result;
}

bool PluginRegistry::checkDependencies(const PluginMetadata& metadata) const {
    // Simple dependency checking - in a real implementation, this would be more sophisticated
    for (const auto& dep : metadata.dependencies) {
        // Check if any loaded plugin provides this dependency
        bool found = false;
        for (const auto& pair : plugins) {
            if (pair.first == dep) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

bool PluginRegistry::resolveDependencies(const PluginMetadata& metadata) const {
    // This would be called during initialization to ensure all dependencies are loaded
    return checkDependencies(metadata);
}

// PluginLoader implementation
std::unique_ptr<NLMPlugin> PluginLoader::loadPlugin(const std::string& filepath) {
    // For now, this is a placeholder - in a real implementation, this would
    // use dynamic library loading (dlopen, dlsym, etc.)
    std::cout << "PluginLoader: Loading plugin from " << filepath << " (placeholder implementation)" << std::endl;
    return nullptr;
}

bool PluginLoader::canLoadPlugin(const std::string& filepath) {
    // Check file extension and other properties
    // In a real implementation, this would verify it's a valid plugin library
    return false;
}

std::vector<std::string> PluginLoader::getPluginFiles(const std::string& directory) const {
    std::vector<std::string> result;
    // In a real implementation, this would scan the directory for plugin files
    return result;
}

// PluginConfigurator implementation
bool PluginConfigurator::configurePlugin(NLMPlugin& plugin, const std::string& configKey, 
                                        const std::string& configValue) {
    // Placeholder implementation
    std::cout << "PluginConfigurator: Configuring " << plugin.getMetadata().name 
              << " with key=" << configKey << ", value=" << configValue << std::endl;
    return true;
}

std::string PluginConfigurator::getPluginConfig(NLMPlugin& plugin, const std::string& configKey) {
    // Placeholder implementation
    std::cout << "PluginConfigurator: Getting config for " << plugin.getMetadata().name 
              << " with key=" << configKey << std::endl;
    return "";
}

bool PluginConfigurator::savePluginConfig(const std::string& pluginName, 
                                          const std::string& configFile) {
    // Placeholder implementation
    std::cout << "PluginConfigurator: Saving config for " << pluginName 
              << " to " << configFile << std::endl;
    return true;
}

bool PluginConfigurator::loadPluginConfig(NLMPlugin& plugin, const std::string& configFile) {
    // Placeholder implementation
    std::cout << "PluginConfigurator: Loading config for " << plugin.getMetadata().name 
              << " from " << configFile << std::endl;
    return true;
}

} // namespace nlm