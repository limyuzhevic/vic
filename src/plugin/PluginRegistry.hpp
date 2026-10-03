#pragma once

#include "PluginBase.hpp"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>

namespace nlm {

class PluginRegistry {
public:
    static PluginRegistry& getInstance() {
        static PluginRegistry instance;
        return instance;
    }
    
    // Register a built-in plugin (compile-time registration)
    template<typename PluginType>
    void registerBuiltInPlugin() {
        auto plugin = std::make_unique<PluginType>();
        PluginMetadata metadata = plugin->getMetadata();
        
        std::lock_guard<std::mutex> lock(mutex);
        builtInPlugins[metadata.name] = std::move(plugin);
        metadataRegistry[metadata.name] = metadata;
    }
    
    // Register an external plugin (runtime registration)
    void registerExternalPlugin(const std::string& name, 
                                const PluginMetadata& metadata,
                                std::unique_ptr<PluginBase> plugin) {
        std::lock_guard<std::mutex> lock(mutex);
        externalPlugins[name] = std::move(plugin);
        metadataRegistry[name] = metadata;
    }
    
    // Load a plugin from library
    std::unique_ptr<PluginBase> loadPlugin(const std::string& libraryPath);
    
    // Get plugin by name
    std::unique_ptr<PluginBase> getPlugin(const std::string& name);
    
    // Check if plugin exists
    bool hasPlugin(const std::string& name) const;
    
    // Get all plugin names
    std::vector<std::string> getPluginNames() const;
    
    // Get plugin metadata
    PluginMetadata getPluginMetadata(const std::string& name) const;
    
    // Initialize all plugins
    void initializeAll(const std::unordered_map<std::string, std::string>& globalConfig);
    
    // Shutdown all plugins
    void shutdownAll();
    
    // Update all plugins
    void updateAll(double timestep);
    
    // Reset all plugins
    void resetAll();
    
    // Statistics
    struct RegistryStats {
        size_t builtInCount = 0;
        size_t externalCount = 0;
        size_t totalMemoryUsage = 0;
        std::atomic<size_t> pluginAccessCount{0};
    };
    
    RegistryStats getStats() const;
    
    // Thread safety
    void lock() const { std::lock_guard<std::mutex> lock(mutex); }
    void unlock() const { std::lock_guard<std::mutex> lock(mutex); }
    
private:
    PluginRegistry() = default;
    ~PluginRegistry() = default;
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    
    mutable std::mutex mutex;
    std::unordered_map<std::string, std::unique_ptr<PluginBase>> builtInPlugins;
    std::unordered_map<std::string, std::unique_ptr<PluginBase>> externalPlugins;
    std::unordered_map<std::string, PluginMetadata> metadataRegistry;
};

} // namespace nlm