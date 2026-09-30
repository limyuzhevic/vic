// NLM Plugin System
// Plugin manager - provides a unified interface for plugin operations

#pragma once

#include "PluginRegistry.hpp"
#include <string>
#include <unordered_map>
#include <any>
#include <vector>

namespace nlm {

// Plugin manager - provides a unified interface for plugin operations
class PluginManager {
private:
    std::shared_ptr<PluginRegistry> registry;
    
public:
    explicit PluginManager(std::shared_ptr<PluginRegistry> registry) : registry(registry) {}
    ~PluginManager() = default;
    
    // Plugin discovery
    std::vector<PluginMetadata> listPlugins() const {
        return registry->getPlugins();
    }
    
    std::vector<PluginMetadata> listPlugins(PluginType type) const {
        return registry->getPlugins(type);
    }
    
    PluginMetadata getPluginInfo(const std::string& name) const {
        return registry->getPluginMetadata(name);
    }
    
    // Plugin lifecycle
    bool loadPlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config = {}) {
        bool loaded = registry->loadPlugin(name);
        if (loaded && !config.empty()) {
            registry->configurePlugin(name, config);
        }
        return loaded;
    }
    
    bool unloadPlugin(const std::string& name) {
        return registry->unloadPlugin(name);
    }
    
    bool activatePlugin(const std::string& name) {
        return registry->activatePlugin(name);
    }
    
    bool deactivatePlugin(const std::string& name) {
        return registry->deactivatePlugin(name);
    }
    
    // Plugin execution
    bool runCommand(const std::string& pluginName, const std::string& command,
                   const std::vector<std::string>& args = {}) {
        return registry->executePluginCommand(pluginName, command, args);
    }
    
    std::any queryPlugin(const std::string& pluginName, const std::string& query) {
        return registry->getPluginResult(pluginName, query);
    }
    
    // Configuration management
    bool configurePlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config) {
        return registry->configurePlugin(name, config);
    }
    
    std::unordered_map<std::string, std::any> getPluginConfig(const std::string& name) const {
        return registry->getPluginConfig(name);
    }
    
    // Plugin validation
    bool validatePlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config) const {
        return registry->validatePluginConfig(name, config);
    }
    
    // Plugin statistics
    size_t getLoadedPluginCount() const {
        size_t count = 0;
        const auto& plugins = registry->getPlugins();
        for (const auto& plugin : plugins) {
            if (registry->isPluginLoaded(plugin.name)) {
                count++;
            }
        }
        return count;
    }
    
    size_t getActivePluginCount() const {
        size_t count = 0;
        const auto& plugins = registry->getPlugins();
        for (const auto& plugin : plugins) {
            if (registry->isPluginActive(plugin.name)) {
                count++;
            }
        }
        return count;
    }
    
    double getTotalPluginMemoryUsage() const {
        double total = 0.0;
        const auto& plugins = registry->getPlugins();
        for (const auto& plugin : plugins) {
            if (registry->isPluginLoaded(plugin.name)) {
                // In a real implementation, we would track actual memory usage
                // For demo, we'll return a placeholder value
                total += 1.0; // 1MB placeholder per plugin
            }
        }
        return total;
    }
    
    // Utility methods
    bool exportPluginList(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        auto plugins = registry->getPlugins();
        file << "Plugin Registry Export" << std::endl;
        file << "=======================" << std::endl;
        file << std::endl;
        
        for (const auto& plugin : plugins) {
            file << "Plugin: " << plugin.name << std::endl;
            file << "  Description: " << plugin.description << std::endl;
            file << "  Version: " << plugin.version << std::endl;
            file << "  Type: " << static_cast<int>(plugin.type) << std::endl;
            file << "  Active: " << (registry->isPluginActive(plugin.name) ? "Yes" : "No") << std::endl;
            file << "  Loaded: " << (registry->isPluginLoaded(plugin.name) ? "Yes" : "No") << std::endl;
            file << std::endl;
        }
        
        return true;
    }
    
    bool importPluginList(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // In a real implementation, we would parse the exported plugin list
        // For demo, we'll just show what would be imported
        std::cout << "Importing plugins from: " << filename << std::endl;
        // This would involve reading plugin metadata and registering them
        
        return true;
    }
    
    bool backupPlugins(const std::string& directory) const {
        // Create backup directory if it doesn't exist
        std::filesystem::create_directories(directory);
        
        auto plugins = registry->getPlugins();
        for (const auto& plugin : plugins) {
            if (registry->isPluginLoaded(plugin.name)) {
                std::string backupFile = directory + "/" + plugin.name + "_backup.json";
                // In a real implementation, we would backup plugin configuration
                std::cout << "Backing up plugin: " << plugin.name << " to " << backupFile << std::endl;
            }
        }
        
        return true;
    }
    
    bool restorePlugins(const std::string& directory) {
        // Check if backup directory exists
        if (!std::filesystem::exists(directory)) {
            return false;
        }
        
        // In a real implementation, we would restore plugin configurations
        std::cout << "Restoring plugins from: " << directory << std::endl;
        
        return true;
    }
};

// Plugin discovery helper functions
namespace PluginDiscovery {
    
    // Find plugins by tag
    std::vector<PluginMetadata> findPluginsByTag(const PluginRegistry& registry, const std::string& tag) {
        return registry.getPluginsByTag(tag);
    }
    
    // Find plugins by type
    std::vector<PluginMetadata> findPluginsByType(const PluginRegistry& registry, PluginType type) {
        return registry.getPlugins(type);
    }
    
    // Get all available tags
    std::vector<std::string> getAllTags(const PluginRegistry& registry) {
        std::vector<std::string> tags;
        const auto& plugins = registry.getPlugins();
        for (const auto& plugin : plugins) {
            for (const auto& tag : plugin.tags) {
                if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
                    tags.push_back(tag);
                }
            }
        }
        return tags;
    }
    
    // Plugin compatibility check
    bool checkPluginCompatibility(const PluginMetadata& plugin, const std::vector<std::string>& requiredTags) {
        for (const auto& requiredTag : requiredTags) {
            if (std::find(plugin.tags.begin(), plugin.tags.end(), requiredTag) == plugin.tags.end()) {
                return false;
            }
        }
        return true;
    }
    
    // Get plugin dependencies
    std::vector<std::string> getPluginDependencies(const PluginMetadata& plugin) {
        // In a real implementation, this would read plugin dependencies from metadata
        // For demo, we'll return a placeholder
        return std::vector<std::string>();
    }
}

} // namespace nlm
