#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <atomic>
#include "PluginManager.hpp"
#include "PluginConfig.hpp"
#include "PluginSecurity.hpp"

namespace nlm {

class PluginSystem {
public:
    // System-wide settings
    struct SystemSettings {
        bool enablePluginSystem = true;
        bool autoDiscoverPlugins = true;
        bool validatePluginsOnLoad = true;
        bool enableHotReload = false;
        float pluginTimeout = 30.0f;  // seconds
        size_t maxMemoryUsage = 1024 * 1024 * 1024;  // 1GB
        std::string pluginConfigFile;
        std::vector<std::string> pluginSearchPaths;
        std::unordered_map<std::string, std::string> pluginDependencies;
    };
    
    // Plugin system state
    struct SystemState {
        bool initialized = false;
        bool running = false;
        std::atomic<size_t> activePlugins{0};
        std::atomic<size_t> failedPlugins{0};
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point lastUpdate;
        float totalSystemUptime = 0.0f;
        size_t totalPluginMemory = 0;
    };
    
    PluginSystem();
    ~PluginSystem();
    
    // System initialization
    bool initialize(const SystemSettings& settings);
    
    // System lifecycle
    bool start();
    bool stop();
    bool pause();
    bool resume();
    
    // Plugin discovery and registration
    bool registerPlugin(const std::string& filepath, PluginType expectedType = PluginType::General);
    bool registerPlugin(const std::string& name, const PluginVersion& version = PluginVersion());
    
    // Global plugin management
    bool enablePlugin(const std::string& instanceId);
    bool disablePlugin(const std::string& instanceId);
    bool unloadPlugin(const std::string& instanceId);
    bool restartPlugin(const std::string& instanceId);
    
    // Plugin configuration
    void setGlobalPluginConfig(const std::shared_ptr<PluginConfig>& config);
    std::shared_ptr<PluginConfig> getGlobalPluginConfig() const;
    
    // Extension point management
    bool registerExtensionPoint(const std::string& name, PluginType type,
                               const std::function<bool(std::shared_ptr<Plugin>)>& validator = nullptr);
    bool registerExtensionPoint(const std::string& name, PluginType type,
                               const std::vector<std::string>& requiredCapabilities);
    
    // Plugin system integration
    std::vector<std::string> getAvailableExtensions() const;
    std::vector<std::string> getAvailablePlugins() const;
    
    // System-wide operations
    void updateAllPlugins(float dt);
    void broadcastSystemMessage(const std::string& message, const std::string& data = "");
    void sendSystemMessage(const std::string& target, const std::string& message, const std::string& data = "");
    
    // System statistics and monitoring
    SystemState getSystemState() const;
    std::string getSystemStatus() const;
    std::vector<std::string> getSystemErrors() const;
    size_t getActivePluginCount() const;
    size_t getTotalPluginCount() const;
    
    // Plugin-specific queries
    std::vector<std::string> getPluginsByType(PluginType type) const;
    std::vector<std::string> getPluginsByExtensionPoint(const std::string& extensionPoint) const;
    std::vector<std::string> getPluginsByCapability(const std::string& capability) const;
    
    // Configuration management
    bool loadPluginConfig(const std::string& filepath);
    bool savePluginConfig(const std::string& filepath) const;
    bool validatePluginConfig() const;
    
    // Security and safety
    bool enablePluginSecurity(bool enable);
    bool isPluginSecurityEnabled() const { return securityEnabled_; }
    bool validatePluginSecurity(std::shared_ptr<Plugin> plugin) const;
    
    // Plugin communication interfaces
    void registerMessageHandler(const std::string& messageType,
                               const std::function<void(const std::string&, const std::string&)>& handler);
    void registerEventHandler(const std::string& eventType,
                             const std::function<void(std::shared_ptr<Plugin>)>& handler);
    
    // Performance optimization
    void optimizePluginPerformance();
    void cleanupInactivePlugins();
    void updatePluginPriorities();
    
    // Debugging and visualization
    std::string getPluginDebugInfo(const std::string& instanceId) const;
    std::string getSystemDebugInfo() const;
    
    // Plugin-specific debugging
    void attachDebugger(std::shared_ptr<class PluginDebugger> debugger);
    void detachDebugger();
    
    // Cleanup and shutdown
    void cleanup();
    void shutdown();
    
    // Singleton access
    static PluginSystem& getInstance();
    static void destroyInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Settings and state
    SystemSettings settings_;
    SystemState state_;
    
    // Security
    bool securityEnabled_ = false;
    std::shared_ptr<PluginSecurity> security_;
    
    // Global configuration
    std::shared_ptr<PluginConfig> globalConfig_;
    
    // Communication
    std::unordered_map<std::string, std::function<void(const std::string&, const std::string&)>> messageHandlers_;
    std::unordered_map<std::string, std::function<void(std::shared_ptr<Plugin>)>> eventHandlers_;
    
    // Debugging
    std::shared_ptr<class PluginDebugger> debugger_;
    
    // Initialize from settings
    bool initializeFromConfig(const std::string& configFile = "");
    
    // Update system state
    void updateSystemState();
    
    // Plugin dependency resolution
    bool resolvePluginDependencies(const std::string& instanceId);
    
    // Plugin lifecycle callbacks
    void onPluginLoaded(std::shared_ptr<Plugin> plugin);
    void onPluginEnabled(std::shared_ptr<Plugin> plugin);
    void onPluginDisabled(std::shared_ptr<Plugin> plugin);
    void onPluginUnloaded(std::shared_ptr<Plugin> plugin);
    
    // Error handling
    void handlePluginError(const std::string& instanceId, const std::string& error);
    void logSystemError(const std::string& error);
};

} // namespace nlm