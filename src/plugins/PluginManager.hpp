#pragma once

// Plugin Manager for NLM (Neural Learning Machine)
// Handles plugin loading, initialization, and lifecycle management

#include "PluginCore.hpp"
#include <thread>
#include <future>
#include <shared_mutex>

namespace nlm {

// Plugin Manager Configuration
struct PluginManagerConfig {
    size_t maxConcurrentLoads{4};
    double loadTimeoutSeconds{30.0};
    double unloadTimeoutSeconds{10.0};
    double updateTimeoutSeconds{1.0};
    bool autoLoadPlugins{true};
    bool autoSaveConfigs{true};
    std::string configFilePath{"plugins_config.json"};
    std::vector<std::string> defaultPluginPaths{"plugins", "user/plugins", "system/plugins"};
    PluginSecurityLevel defaultSecurityLevel{PluginSecurityLevel::Medium};
    size_t maxPluginMemoryUsageMB{100};
    double maxCPUUsagePercent{10.0};
    bool enablePluginCaching{true};
    size_t cacheSize{100};
};

// Plugin Manager
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Disable copying, enable moving
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) noexcept;
    PluginManager& operator=(PluginManager&&) noexcept;
    
    // Initialize plugin manager
    bool initialize(const PluginManagerConfig& config = PluginManagerConfig());
    
    // Shutdown plugin manager
    void shutdown();
    
    // Check if plugin manager is active
    bool isActive() const;
    
    // Plugin loading
    bool loadPlugin(const std::string& name);
    bool loadPluginFromFile(const std::filesystem::path& filepath);
    bool loadPluginFromDirectory(const std::filesystem::path& directory);
    std::future<bool> loadPluginAsync(const std::string& name);
    
    // Plugin unloading
    bool unloadPlugin(const std::string& name);
    std::future<bool> unloadPluginAsync(const std::string& name);
    
    // Plugin configuration
    bool configurePlugin(const std::string& name, const std::map<std::string, PluginConfigValue>& config);
    std::map<std::string, PluginConfigValue> getPluginConfig(const std::string& name) const;
    bool savePluginConfig(const std::string& name, const std::string& filepath) const;
    bool loadPluginConfig(const std::string& name, const std::string& filepath);
    
    // Plugin state management
    bool savePluginState(const std::string& name, const std::string& filepath) const;
    bool loadPluginState(const std::string& name, const std::string& filepath);
    
    // Plugin lifecycle management
    bool startPlugin(const std::string& name);
    bool stopPlugin(const std::string& name);
    bool pausePlugin(const std::string& name);
    bool resumePlugin(const std::string& name);
    
    PluginLoadStatus getPluginStatus(const std::string& name) const;
    PluginLoadStatus getPluginLoadStatus(const std::string& name) const;
    std::chrono::system_clock::time_point getPluginStartTime(const std::string& name) const;
    std::chrono::system_clock::time_point getPluginStopTime(const std::string& name) const;
    double getPluginUptime(const std::string& name) const;
    
    // Plugin discovery and registration
    std::vector<std::string> discoverPlugins(const std::filesystem::path& directory = std::filesystem::path());
    std::vector<std::string> getAvailablePluginTypes() const;
    std::vector<std::string> getRegisteredPluginNames() const;
    std::vector<std::string> getActivePluginNames() const;
    std::vector<std::string> getLoadedPluginNames() const;
    
    // Plugin information
    Plugin* getPlugin(const std::string& name);
    const Plugin* getPlugin(const std::string& name) const;
    PluginMetadata getPluginMetadata(const std::string& name) const;
    PluginCapabilities getPluginCapabilities(const std::string& name) const;
    std::vector<std::string> getPluginDependencies(const std::string& name) const;
    PluginSecurityLevel getPluginSecurityLevel(const std::string& name) const;
    
    // Plugin system management
    bool registerPluginFactory(const std::string& pluginType, std::unique_ptr<PluginFactory> factory);
    bool unregisterPluginFactory(const std::string& pluginType);
    PluginFactory* getPluginFactory(const std::string& pluginType);
    std::vector<std::string> getRegisteredPluginTypes() const;
    
    // Plugin path management
    bool addPluginPath(const std::filesystem::path& path);
    bool removePluginPath(const std::filesystem::path& path);
    std::vector<std::filesystem::path> getPluginPaths() const;
    
    // Plugin filtering and sorting
    std::vector<Plugin*> getPluginsByCategory(const std::string& category);
    std::vector<Plugin*> getPluginsByCapability(const std::string& capability);
    std::vector<Plugin*> getPluginsByTag(const std::string& tag);
    std::vector<Plugin*> getPluginsBySecurityLevel(PluginSecurityLevel level);
    std::vector<Plugin*> getCompatiblePlugins(const PluginMetadata& pluginMetadata) const;
    
    // Plugin validation and security
    bool validatePlugin(const std::string& name) const;
    bool verifyPlugin(const std::string& name) const;
    bool checkPluginDependencies(const std::string& name) const;
    bool resolvePluginDependencies() const;
    std::vector<std::string> getPluginValidationErrors(const std::string& name) const;
    
    // Plugin events
    void addEventCallback(PluginEventCallback callback);
    void removeEventCallback(PluginEventCallback callback);
    void broadcastPluginEvent(const PluginEvent& event);
    
    // Plugin statistics and monitoring
    PluginStatistics getPluginStatistics(const std::string& name) const;
    PluginSystemStatistics getSystemStatistics() const;
    double getAverageLoadTime(const std::string& name) const;
    double getAverageUnloadTime(const std::string& name) const;
    
    // System configuration
    void setConfig(const PluginManagerConfig& config);
    PluginManagerConfig getConfig() const;
    
    // System commands
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool restartPlugin(const std::string& name);
    bool refreshPlugin(const std::string& name);
    bool updateAllPlugins(double deltaTime);
    
    // System maintenance
    bool cleanPluginCache();
    bool validatePluginSystem();
    bool repairPluginSystem();
    bool optimizePluginSystem();
    
    // System information
    std::string getSystemStatus() const;
    std::string getSystemInfo() const;
    std::vector<std::string> getSystemMessages() const;
    std::vector<std::string> getSystemWarnings() const;
    std::vector<std::string> getSystemErrors() const;
    
    // System commands
    bool exportPluginSystem(const std::string& filename, const std::string& format = "json") const;
    bool importPluginSystem(const std::string& filename, const std::string& format = "json");
    
    // Plugin commands
    bool executePluginCommand(const std::string& name, const std::string& command, 
                             const std::map<std::string, std::string>& args = {});
    
    // System commands
    bool executeSystemCommand(const std::string& command, 
                             const std::map<std::string, std::string>& args = {});
    
    // Plugin help and documentation
    std::string getPluginHelp(const std::string& name) const;
    std::string getSystemHelp() const;
    std::string getPluginDocumentation(const std::string& name) const;
    
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
    virtual void onPluginLoaded(const std::string& name);
    virtual void onPluginUnloaded(const std::string& name);
    virtual void onPluginStarted(const std::string& name);
    virtual void onPluginStopped(const std::string& name);
    virtual void onPluginError(const std::string& name, const std::string& error);
    virtual void onPluginWarning(const std::string& name, const std::string& warning);
    
    // System lifecycle callbacks
    virtual void onSystemInitialized();
    virtual void onSystemShutdown();
    virtual void onSystemError(const std::string& error);
    virtual void onSystemWarning(const std::string& warning);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool loadPluginInternal(const std::string& name);
    bool unloadPluginInternal(const std::string& name);
    void schedulePluginLoad(const std::string& name);
    void schedulePluginUnload(const std::string& name);
    void processPluginQueue();
    bool validatePluginPaths() const;
    std::filesystem::path getPluginFilePath(const std::string& name) const;
    std::vector<std::filesystem::path> findPluginFiles(const std::filesystem::path& directory) const;
    bool loadPluginFromFileInternal(const std::filesystem::path& filepath);
    std::unique_ptr<Plugin> createPluginFromFile(const std::filesystem::path& filepath);
    std::unique_ptr<Plugin> createPluginFromFactory(const std::string& pluginType, const std::string& name);
    bool checkSecurityConstraints(const PluginMetadata& metadata) const;
    void logPluginEvent(const PluginEvent& event);
    void updateSystemStatistics();
    bool cleanupPluginResources(const std::string& name);
    bool validatePluginDependencies(const std::string& name) const;
    void resolveDependencyOrder();
    std::vector<std::string> getLoadOrder(const std::vector<std::string>& pluginNames) const;
};

} // namespace nlm