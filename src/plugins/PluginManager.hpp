#pragma once

#include "Plugin.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Disable copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    // Plugin lifecycle
    bool loadPlugin(const std::string& name);
    bool loadPlugin(const std::string& name, const std::map<std::string, std::string>& config);
    bool unloadPlugin(const std::string& name);
    bool isPluginLoaded(const std::string& name) const;
    
    // Plugin information
    std::vector<std::string> getLoadedPlugins() const;
    std::string getPluginName(const std::string& name) const;
    std::string getPluginVersion(const std::string& name) const;
    std::string getPluginDescription(const std::string& name) const;
    PluginType getPluginType(const std::string& name) const;
    
    // Plugin configuration
    bool configurePlugin(const std::string& name, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& name) const;
    
    // Plugin lifecycle management
    bool initializeAllPlugins(const std::map<std::string, std::map<std::string, std::string>>& configs = {});
    bool shutdownAllPlugins();
    bool isAllPluginsInitialized() const;
    
    // Plugin dependency management
    bool checkPluginDependencies(const std::string& name) const;
    std::vector<std::string> getPluginDependencies(const std::string& name) const;
    std::vector<std::string> getDependentPlugins(const std::string& name) const;
    
    // Plugin component access
    std::shared_ptr<IPlugin> getPlugin(const std::string& name) const;
    std::vector<std::shared_ptr<IPlugin>> getPluginsByType(PluginType type) const;
    
    // Brain integration
    void onBrainInitialized(std::shared_ptr<class Brain> brain);
    void onExperimentStarted(const std::string& experimentName);
    void onExperimentCompleted(const std::string& experimentName);
    void onStepCompleted(size_t step);
    
    // Plugin discovery and statistics
    std::vector<std::string> discoverAvailablePlugins() const;
    std::map<std::string, std::string> getPluginStatistics(const std::string& name) const;
    
    // Plugin validation
    bool validatePlugin(const std::string& name) const;
    
    // Configuration file support
    bool loadPluginConfigsFromFile(const std::string& filepath);
    bool savePluginConfigsToFile(const std::string& filepath) const;
    
    // Plugin commands and API
    bool executePluginCommand(const std::string& name, const std::string& command, const std::string& parameters = "");
    std::string queryPluginAPI(const std::string& name, const std::string& api, const std::string& parameters = "");
    
    // Plugin version management
    bool updatePlugin(const std::string& name, const std::string& version, const std::string& updateFile);
    std::string getPluginVersion(const std::string& name) const;
    bool isPluginUpdateAvailable(const std::string& name) const;
    
    // Plugin state management
    enum class PluginState {
        NotLoaded,
        Loading,
        Loaded,
        Initializing,
        Initialized,
        ShuttingDown,
        Error
    };
    
    PluginState getPluginState(const std::string& name) const;
    std::map<std::string, PluginState> getAllPluginStates() const;
    
    // Plugin logging and debugging
    void enablePluginLogging(bool enable);
    bool isPluginLoggingEnabled() const;
    void logPluginMessage(const std::string& name, const std::string& message, int level = 0);
    
    // Plugin security
    void setPluginSecurityLevel(const std::string& name, int level);
    int getPluginSecurityLevel(const std::string& name) const;
    
    // Plugin hot-reloading support
    bool enableHotReloading(bool enable);
    bool isHotReloadingEnabled() const;
    bool reloadPlugin(const std::string& name);
    
    // Plugin metadata and documentation
    std::string getPluginHelp(const std::string& name) const;
    std::vector<std::string> getPluginTags(const std::string& name) const;
    std::map<std::string, std::string> getPluginMetadata(const std::string& name) const;
    
    // Plugin performance monitoring
    double getPluginLoadTime(const std::string& name) const;
    double getPluginExecutionTime(const std::string& name) const;
    size_t getPluginMemoryUsage(const std::string& name) const;
    
    // Plugin health monitoring
    bool isPluginHealthy(const std::string& name) const;
    std::string getPluginHealthStatus(const std::string& name) const;
    std::vector<std::string> getPluginIssues(const std::string& name) const;
    
    // Plugin isolation and sandboxing
    void enablePluginIsolation(bool enable);
    bool isPluginIsolationEnabled() const;
    std::string getPluginSandboxPath(const std::string& name) const;
    
    // Plugin backup and restore
    bool backupPlugin(const std::string& name, const std::string& backupPath);
    bool restorePlugin(const std::string& name, const std::string& backupPath);
    std::vector<std::string> getPluginBackups(const std::string& name) const;
    
    // Plugin analytics and monitoring
    std::map<std::string, double> getPluginAnalytics(const std::string& name) const;
    void recordPluginUsage(const std::string& name, const std::string& action, double duration = 0.0);
    std::vector<std::pair<std::string, double>> getPluginUsageHistory(const std::string& name) const;
    
    // Plugin marketplace integration (future)
    // bool searchPluginMarketplace(const std::string& query);
    // bool downloadPlugin(const std::string& pluginId, const std::string& version);
    
    // Plugin migration and compatibility
    bool checkPluginCompatibility(const std::string& name, const std::string& version) const;
    std::vector<std::string> getPluginDependencies(const std::string& name, const std::string& version) const;
    bool migratePlugin(const std::string& name, const std::string& targetVersion);
    
    // Plugin event system
    using PluginEventCallback = std::function<void(const std::string&, const std::string&, const std::map<std::string, std::string>&)>;
    void subscribeToPluginEvent(const std::string& eventType, PluginEventCallback callback);
    void unsubscribeFromPluginEvent(const std::string& eventType, PluginEventCallback callback);
    void emitPluginEvent(const std::string& eventType, const std::string& pluginName, const std::map<std::string, std::string>& data);
    
    // Plugin configuration schema
    bool validatePluginConfig(const std::string& name, const std::map<std::string, std::string>& config) const;
    std::map<std::string, std::string> getPluginConfigSchema(const std::string& name) const;
    std::vector<std::string> getPluginConfigOptions(const std::string& name) const;
    
    // Plugin uninstallation support
    bool uninstallPlugin(const std::string& name);
    bool isPluginInstalled(const std::string& name) const;
    
    // Plugin packaging and distribution
    std::string packagePlugin(const std::string& name, const std::string& outputPath) const;
    bool installPluginPackage(const std::string& packagePath);
    
    // Plugin debugging and development tools
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    std::string getPluginDebugInfo(const std::string& name) const;
    std::vector<std::string> getPluginStackTrace(const std::string& name) const;
    
    // Plugin recovery and fault tolerance
    bool enableFaultTolerance(bool enable);
    bool isFaultToleranceEnabled() const;
    bool recoverPlugin(const std::string& name);
    bool enablePluginRecovery(const std::string& name, bool enable);
    
    // Plugin resource management
    bool limitPluginResources(const std::string& name, const std::map<std::string, double>& limits);
    std::map<std::string, double> getPluginResourceUsage(const std::string& name) const;
    bool cleanupPluginResources(const std::string& name);
    
    // Plugin lifecycle callbacks
    using PluginInitCallback = std::function<void(const std::string&)>;
    using PluginShutdownCallback = std::function<void(const std::string&)>;
    using PluginErrorCallback = std::function<void(const std::string&, const std::string&)>; // (plugin name, error message)
    
    void setPluginInitCallback(PluginInitCallback callback);
    void setPluginShutdownCallback(PluginShutdownCallback callback);
    void setPluginErrorCallback(PluginErrorCallback callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace nlm