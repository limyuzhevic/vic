//! Plugin Manager for NLM Neural Simulation Project
//! 
//! This module implements the PluginManager class which is responsible for:
//! - Loading, managing, and unloading plugins
//! - Thread-safe plugin operations
//! - Plugin lifecycle management
//! - Dependency resolution
//! - Security management
//! - Event-driven plugin communication
//! - Integration with core NLM systems via hooks
//! 
//! The PluginManager acts as the central coordinator for all plugin operations,
//! providing a unified interface for plugin management while maintaining
//! thread safety and security.

#pragma once

#include "PluginBase.hpp"
#include <unordered_map>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <shared_mutex>
#include <filesystem>

namespace nlm {

class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Disable copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    // Enable moving
    PluginManager(PluginManager&&) noexcept = default;
    PluginManager& operator=(PluginManager&&) noexcept = default;
    
    // Plugin management
    bool loadPlugin(const std::string& filePath);
    bool loadPlugin(const PluginMetadata& metadata, std::unique_ptr<PluginBase> plugin);
    bool unloadPlugin(const std::string& pluginName);
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    
    // Plugin information
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getEnabledPlugins() const;
    PluginState getPluginState(const std::string& pluginName) const;
    const PluginMetadata* getPluginMetadata(const std::string& pluginName) const;
    const PluginConfig* getPluginConfig(const std::string& pluginName) const;
    PluginBase* getPlugin(const std::string& pluginName);
    
    // Plugin lifecycle
    bool initializePlugin(const std::string& pluginName);
    void startAllPlugins();
    void updateAllPlugins(double deltaTime);
    void stopAllPlugins();
    bool shutdownPlugin(const std::string& pluginName);
    
    // Dependency management
    bool resolveDependencies(const std::string& pluginName);
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;
    std::vector<std::string> getUnsatisfiedDependencies(const std::string& pluginName) const;
    
    // Plugin operations
    bool configurePlugin(const std::string& pluginName, const PluginConfig& config);
    bool updatePluginConfiguration(const std::string& pluginName, const std::string& key, const std::any& value);
    std::string serializePluginConfiguration(const std::string& pluginName) const;
    bool deserializePluginConfiguration(const std::string& pluginName, const std::string& data);
    
    // Event handling
    void addEventListener(std::shared_ptr<IEventListener> listener);
    void removeEventListener(const std::string& listenerName);
    void notifyEvent(const PluginEventData& event);
    void notifyHook(const HookEventData& hook);
    
    // Registry integration
    bool registerPluginWithRegistry(const PluginMetadata& metadata);
    bool unregisterPluginFromRegistry(const std::string& pluginName);
    IPluginRegistry* getRegistry() const;
    
    // Security and permissions
    bool checkSecurityPermissions(const std::string& pluginName) const;
    std::vector<std::string> getRequiredPermissions(const std::string& pluginName) const;
    std::vector<std::string> getGrantedPermissions(const std::string& pluginName) const;
    
    // Configuration management
    bool savePluginConfigurations(const std::string& filePath) const;
    bool loadPluginConfigurations(const std::string& filePath);
    bool validateAllPluginConfigurations() const;
    
    // Statistics and monitoring
    size_t getLoadedPluginCount() const;
    size_t getEnabledPluginCount() const;
    size_t getFailedPluginCount() const;
    double getAveragePluginLoadTime() const;
    std::unordered_map<std::string, double> getPluginLoadTimes() const;
    
    // Plugin type access
    std::vector<std::string> getPluginsByType(const std::string& pluginType) const;
    std::vector<std::string> getPluginsByTag(const std::string& tag) const;
    
    // Thread safety
    void waitForPluginOperations();
    bool isOperationPending() const;
    
    // System operations
    void pauseAllPlugins();
    void resumeAllPlugins();
    bool shutdownAllPlugins();
    void clearAllPlugins();
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directoryPath) const;
    bool validatePluginFile(const std::string& filePath) const;
    
    // Advanced operations
    bool replacePlugin(const std::string& pluginName, const std::string& newFilePath);
    bool updatePlugin(const std::string& pluginName);
    std::vector<std::string> getPluginUpdateCandidates() const;
    
    // Error handling
    std::string getLastError(const std::string& pluginName = "") const;
    std::vector<std::string> getRecentErrors() const;
    void clearErrors(const std::string& pluginName = "");
    
    // Plugin hooks integration
    void registerHook(const PluginHook& hook, const std::function<bool(const HookEventData&)>& callback);
    void unregisterHook(const PluginHook& hook, const std::string& pluginName = "");
    bool invokeHook(const PluginHook& hook, HookEventData& data, const std::string& pluginName = "");
    
    // Statistics collection
    struct PluginStats {
        std::string name;
        PluginState state;
        double loadTime;
        double lastUpdateTime;
        size_t updateCount;
        std::chrono::system_clock::time_point lastUpdated;
        std::chrono::system_clock::time_point created;
        size_t memoryUsage;
        
        PluginStats(const std::string& n) : name(n), state(PluginState::Unloaded),
            loadTime(0.0), lastUpdateTime(0.0), updateCount(0), created(std::chrono::system_clock::now()) {}
    };
    
    PluginStats getPluginStats(const std::string& pluginName) const;
    std::vector<PluginStats> getAllPluginStats() const;
    
    // Configuration integration
    void setConfigurationSystem(std::shared_ptr<PluginConfiguration> configSystem);
    void setSecurityManager(std::shared_ptr<PluginSecurityManager> securityManager);
    void setUpdateManager(std::shared_ptr<PluginUpdateManager> updateManager);
    void setHookSystem(std::shared_ptr<HookSystem> hookSystem);
    void setEventSystem(std::shared_ptr<EventSystem> eventSystem);
    
    // Plugin lifecycle callbacks
    using PluginLoadCallback = std::function<void(const std::string&, bool, const std::string&)>;
    using PluginStateChangeCallback = std::function<void(const std::string&, PluginState, PluginState, const std::string&)>;
    using PluginEventCallback = std::function<void(const PluginEventData&)>; 
    
    void addLoadCallback(const PluginLoadCallback& callback);
    void addStateChangeCallback(const PluginStateChangeCallback& callback);
    void addEventCallback(const PluginEventCallback& callback);
    
    // Debug and monitoring
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void logPluginState(const std::string& pluginName, PluginState state) const;
    void logPluginError(const std::string& pluginName, const std::string& error) const;
    
    // Performance monitoring
    struct PerformanceMetrics {
        double averageLoadTime;
        double averageUpdateTime;
        size_t totalUpdates;
        size_t failedLoads;
        size_t failedUpdates;
        double systemUptime;
        
        PerformanceMetrics() : averageLoadTime(0.0), averageUpdateTime(0.0),
            totalUpdates(0), failedLoads(0), failedUpdates(0), systemUptime(0.0) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeImpl();
    void cleanupImpl();
    
    // Thread-safe operations
    template<typename T>
    T withPluginLock(T func) {
        std::shared_lock<std::shared_mutex> lock(pluginMutex_);
        return func();
    }
    
    template<typename T>
    T withPluginWriteLock(T func) {
        std::unique_lock<std::shared_mutex> lock(pluginWriteMutex_);
        return func();
    }
}; // namespace nlm