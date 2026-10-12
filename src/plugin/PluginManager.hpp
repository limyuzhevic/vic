#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <functional>
#include <mutex>
#include <atomic>
#include "Plugin.hpp"
#include "PluginLoader.hpp"
#include "PluginAPI.hpp"

namespace nlm {

class PluginManager {
public:
    // Plugin configuration
    struct PluginSettings {
        std::string pluginDirectory;
        bool autoLoad = true;
        bool enableAllPlugins = false;
        bool hotReload = false;
        float unloadTimeout = 5.0f;  // seconds
        size_t maxConcurrentLoads = 4;
        bool validatePlugins = true;
        bool sandboxPlugins = false;
        bool logPluginActivity = true;
    };
    
    PluginManager();
    ~PluginManager();
    
    // Initialize plugin system
    bool initialize(const PluginSettings& settings);
    
    // Plugin management
    PluginLoader::LoadResult loadPlugin(const std::string& filepath, PluginType expectedType = PluginType::General);
    PluginLoader::LoadResult loadPlugin(const std::string& name, const PluginVersion& version = PluginVersion());
    
    // Plugin lifecycle management
    bool enablePlugin(const std::string& instanceId);
    bool disablePlugin(const std::string& instanceId);
    bool unloadPlugin(const std::string& instanceId);
    bool restartPlugin(const std::string& instanceId);
    
    // Plugin discovery
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getEnabledPlugins() const;
    
    // Plugin information
    std::shared_ptr<Plugin> getPlugin(const std::string& instanceId) const;
    PluginMetadata getPluginMetadata(const std::string& instanceId) const;
    PluginStatus getPluginStatus(const std::string& instanceId) const;
    
    // Plugin configuration
    void configurePlugin(const std::string& instanceId, const std::shared_ptr<class PluginConfig>& config);
    std::shared_ptr<class PluginConfig> getPluginConfig(const std::string& instanceId) const;
    
    // Extension point management
    void registerExtensionPoint(const std::string& name, PluginType type, 
                               const std::function<bool(std::shared_ptr<Plugin>)>& validator = nullptr);
    bool requestExtensionPoint(const std::string& name, std::shared_ptr<Plugin> plugin);
    
    // Plugin communication
    void broadcastMessage(const std::string& message, const std::string& data = "");
    void sendMessage(const std::string& target, const std::string& message, const std::string& data = "");
    
    // Update all active plugins
    void update(float dt);
    
    // Plugin system state
    bool isInitialized() const { return initialized_; }
    void setPluginDirectory(const std::string& directory);
    std::string getPluginDirectory() const { return settings_.pluginDirectory; }
    
    // Statistics
    size_t getLoadCount() const { return totalLoadCount_; }
    size_t getUnloadCount() const { return totalUnloadCount_; }
    float getAverageLoadTime() const { return averageLoadTime_; }
    
    // Error handling
    std::vector<std::string> getRecentErrors() const;
    void clearRecentErrors();
    
    // Plugin-specific getters/setters
    std::vector<std::shared_ptr<Plugin>> getPluginsByType(PluginType type) const;
    std::vector<std::shared_ptr<Plugin>> getPluginsByExtensionPoint(const std::string& extensionPoint) const;
    
    // Cleanup
    void cleanup();
    void shutdown();
    
    // Plugin enumeration
    std::vector<std::string> enumeratePlugins() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Settings
    PluginSettings settings_;
    bool initialized_ = false;
    
    // Statistics
    std::atomic<size_t> totalLoadCount_{0};
    std::atomic<size_t> totalUnloadCount_{0};
    std::atomic<float> averageLoadTime_{0.0f};
    
    // Error tracking
    std::vector<std::string> recentErrors_;
    mutable std::mutex errorsMutex_;
};

} // namespace nlm