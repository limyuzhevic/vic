#pragma once

// Plugin Lifecycle Manager for NLM (Neural Learning Machine)
// Handles plugin lifecycle management (load, start, update, stop, unload)

#include "PluginCore.hpp"
#include <thread>
#include <future>
#include <chrono>
#include <atomic>
#include <queue>

namespace nlm {

// Plugin Lifecycle Manager Configuration
struct PluginLifecycleConfig {
    double updateIntervalSeconds{0.1};
    double loadTimeoutSeconds{30.0};
    double unloadTimeoutSeconds{10.0};
    double startTimeoutSeconds{5.0};
    double stopTimeoutSeconds{5.0};
    bool enableAutomaticLifecycleManagement{true};
    bool enablePluginMonitoring{true};
    bool enablePluginRecovery{true};
    double recoveryTimeoutSeconds{60.0};
    size_t maxPluginUptimeHours{24};
    bool enablePluginProfiling{true};
    size_t maxPluginHistorySize{1000};
    bool enableLifecycleLogging{true};
    std::string lifecycleLogFile{"plugin_lifecycle.log"};
};

// Plugin Lifecycle Event
struct PluginLifecycleEvent {
    std::string pluginName;
    PluginLoadStatus status;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::map<std::string, std::string> metadata;
};

// Plugin Lifecycle Callback
using PluginLifecycleCallback = std::function<void(const PluginLifecycleEvent&)>;

// Plugin Lifecycle Manager
class PluginLifecycleManager {
public:
    PluginLifecycleManager();
    ~PluginLifecycleManager();
    
    // Disable copying, enable moving
    PluginLifecycleManager(const PluginLifecycleManager&) = delete;
    PluginLifecycleManager& operator=(const PluginLifecycleManager&) = delete;
    PluginLifecycleManager(PluginLifecycleManager&&) noexcept;
    PluginLifecycleManager& operator=(PluginLifecycleManager&&) noexcept;
    
    // Initialize plugin lifecycle manager
    bool initialize(const PluginLifecycleConfig& config = PluginLifecycleConfig());
    
    // Shutdown plugin lifecycle manager
    void shutdown();
    
    // Check if plugin lifecycle manager is active
    bool isActive() const;
    
    // Plugin lifecycle management
    bool loadPlugin(const std::string& name);
    bool loadPlugin(const std::string& name, const std::map<std::string, PluginConfigValue>& config);
    std::future<bool> loadPluginAsync(const std::string& name);
    std::future<bool> loadPluginAsync(const std::string& name, const std::map<std::string, PluginConfigValue>& config);
    
    bool unloadPlugin(const std::string& name);
    std::future<bool> unloadPluginAsync(const std::string& name);
    
    bool startPlugin(const std::string& name);
    std::future<bool> startPluginAsync(const std::string& name);
    
    bool stopPlugin(const std::string& name);
    std::future<bool> stopPluginAsync(const std::string& name);
    
    bool pausePlugin(const std::string& name);
    std::future<bool> pausePluginAsync(const std::string& name);
    
    bool resumePlugin(const std::string& name);
    std::future<bool> resumePluginAsync(const std::string& name);
    
    bool restartPlugin(const std::string& name);
    std::future<bool> restartPluginAsync(const std::string& name);
    
    // Plugin state management
    PluginLoadStatus getPluginStatus(const std::string& name) const;
    std::chrono::system_clock::time_point getPluginStartTime(const std::string& name) const;
    std::chrono::system_clock::time_point getPluginStopTime(const std::string& name) const;
    std::chrono::system_clock::time_point getPluginLastUpdateTime(const std::string& name) const;
    double getPluginUptime(const std::string& name) const;
    double getPluginRuntime(const std::string& name) const;
    
    bool isPluginLoaded(const std::string& name) const;
    bool isPluginStarted(const std::string& name) const;
    bool isPluginRunning(const std::string& name) const;
    bool isPluginStopped(const std::string& name) const;
    bool isPluginUnloaded(const std::string& name) const;
    
    // Plugin lifecycle control
    bool forceLoadPlugin(const std::string& name);
    bool forceUnloadPlugin(const std::string& name);
    bool forceStartPlugin(const std::string& name);
    bool forceStopPlugin(const std::string& name);
    
    // Plugin lifecycle monitoring
    bool monitorPlugin(const std::string& name, double intervalSeconds = 1.0);
    bool stopMonitoringPlugin(const std::string& name);
    bool isPluginMonitored(const std::string& name) const;
    std::chrono::system_clock::time_point getPluginLastMonitoringTime(const std::string& name) const;
    
    // Plugin lifecycle events
    void addLifecycleCallback(PluginLifecycleCallback callback);
    void removeLifecycleCallback(PluginLifecycleCallback callback);
    void broadcastLifecycleEvent(const PluginLifecycleEvent& event);
    
    // Plugin configuration
    bool configurePlugin(const std::string& name, const std::map<std::string, PluginConfigValue>& config);
    std::map<std::string, PluginConfigValue> getPluginConfig(const std::string& name) const;
    
    // Plugin statistics
    PluginStatistics getPluginStatistics(const std::string& name) const;
    std::vector<PluginLifecycleEvent> getPluginLifecycleHistory(const std::string& name) const;
    std::vector<PluginLifecycleEvent> getSystemLifecycleEvents() const;
    
    // System lifecycle management
    bool updateAllPlugins(double deltaTime);
    bool updatePlugin(const std::string& name, double deltaTime);
    bool processLifecycleQueue();
    bool cleanPluginHistory();
    
    // Plugin recovery and error handling
    bool recoverPlugin(const std::string& name);
    bool recoverAllFailedPlugins();
    bool handlePluginError(const std::string& name, const std::string& error);
    bool isPluginRecovered(const std::string& name) const;
    
    // Plugin lifecycle validation
    bool validatePluginState(const std::string& name) const;
    bool validateAllPluginStates() const;
    std::vector<std::string> getPluginValidationErrors(const std::string& name) const;
    
    // Plugin lifecycle statistics
    PluginSystemStatistics getSystemStatistics() const;
    double getAverageLoadTime(const std::string& name) const;
    double getAverageUnloadTime(const std::string& name) const;
    double getAverageStartTime(const std::string& name) const;
    double getAverageStopTime(const std::string& name) const;
    
    // System configuration
    void setConfig(const PluginLifecycleConfig& config);
    PluginLifecycleConfig getConfig() const;
    
    // System commands
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool restartPlugin(const std::string& name);
    bool refreshPlugin(const std::string& name);
    
    // System maintenance
    bool cleanupPluginResources(const std::string& name);
    bool cleanupAllPluginResources();
    bool validatePluginDependencies(const std::string& name) const;
    bool resolvePluginDependencies();
    bool optimizePluginLifecycle();
    
    // System information
    std::string getLifecycleStatus() const;
    std::string getLifecycleInfo() const;
    std::vector<std::string> getLifecycleMessages() const;
    std::vector<std::string> getLifecycleWarnings() const;
    std::vector<std::string> getLifecycleErrors() const;
    
    // System commands
    bool exportLifecycleData(const std::string& filename, const std::string& format = "json") const;
    bool importLifecycleData(const std::string& filename, const std::string& format = "json");
    
    // System about information
    std::string getAbout() const;
    std::string getVersion() const;
    std::string getBuildInfo() const;
    
    // Plugin integration management
    bool integratePlugins();
    bool removeIntegration(const std::string& name1, const std::string& name2);
    std::vector<std::pair<std::string, std::string>> getActiveIntegrations() const;
    
    // Memory management
    bool optimizeMemoryUsage();
    bool cleanupUnusedPlugins();
    size_t getPluginMemoryUsage(const std::string& name) const;
    double getPluginCPUUsage(const std::string& name) const;
    
    // Plugin lifecycle callbacks
    virtual void onPluginLoadStarted(const std::string& name);
    virtual void onPluginLoadCompleted(const std::string& name, bool success);
    virtual void onPluginLoadFailed(const std::string& name, const std::string& error);
    
    virtual void onPluginStartRequested(const std::string& name);
    virtual void onPluginStartCompleted(const std::string& name, bool success);
    virtual void onPluginStartFailed(const std::string& name, const std::string& error);
    
    virtual void onPluginStopRequested(const std::string& name);
    virtual void onPluginStopCompleted(const std::string& name, bool success);
    virtual void onPluginStopFailed(const std::string& name, const std::string& error);
    
    virtual void onPluginUnloadRequested(const std::string& name);
    virtual void onPluginUnloadCompleted(const std::string& name, bool success);
    virtual void onPluginUnloadFailed(const std::string& name, const std::string& error);
    
    virtual void onPluginUpdated(const std::string& name, double deltaTime);
    virtual void onPluginRecovered(const std::string& name);
    
    // System lifecycle callbacks
    virtual void onSystemInitialized();
    virtual void onSystemShutdown();
    virtual void onSystemError(const std::string& error);
    virtual void onSystemWarning(const std::string& warning);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool loadPluginInternal(const std::string& name, const std::map<std::string, PluginConfigValue>& config);
    bool unloadPluginInternal(const std::string& name);
    bool startPluginInternal(const std::string& name);
    bool stopPluginInternal(const std::string& name);
    bool pausePluginInternal(const std::string& name);
    bool resumePluginInternal(const std::string& name);
    
    void updatePluginState(const std::string& name, PluginLoadStatus status, const std::string& message = "");
    void addLifecycleEvent(const std::string& name, PluginLoadStatus status, const std::string& message = "");
    
    bool validatePluginDependenciesInternal(const std::string& name) const;
    bool resolveDependencyOrderInternal();
    std::vector<std::string> getLoadOrderInternal(const std::vector<std::string>& pluginNames) const;
    
    void schedulePluginLoad(const std::string& name, const std::map<std::string, PluginConfigValue>& config);
    void schedulePluginUnload(const std::string& name);
    void schedulePluginStart(const std::string& name);
    void schedulePluginStop(const std::string& name);
    void schedulePluginPause(const std::string& name);
    void schedulePluginResume(const std::string& name);
    
    void processLifecycleQueue();
    void monitorActivePlugins();
    
    bool checkPluginTimeout(const std::string& name, PluginLoadStatus status) const;
    void handlePluginTimeout(const std::string& name, PluginLoadStatus status);
    
    bool cleanupPluginResourcesInternal(const std::string& name);
    bool validatePluginStateInternal(const std::string& name) const;
    
    void updateSystemStatistics();
    void updatePluginStatistics(const std::string& name);
    
    bool checkPluginHealth(const std::string& name) const;
    bool performHealthCheck(const std::string& name);
    
    std::future<bool> executeAsyncOperation(std::function<bool()> operation, double timeoutSeconds = 5.0);
    
    void logLifecycleMessage(const std::string& message, const std::string& level = "info");
    void logLifecycleEvent(const PluginLifecycleEvent& event);
};

} // namespace nlm