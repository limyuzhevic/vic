#pragma once

// Plugin Registry for NLM (Neural Learning Machine)
// Handles plugin discovery and registration

#include "PluginCore.hpp"
#include <unordered_map>
#include <mutex>
#include <algorithm>

namespace nlm {

// Plugin Registry Configuration
struct PluginRegistryConfig {
    size_t maxRegisteredPlugins{1000};
    bool autoRegisterPlugins{true};
    bool enablePluginValidation{true};
    bool enablePluginVersionChecking{true};
    bool allowMultipleVersions{false};
    std::string pluginCacheDir{ ".plugin_cache" };
    bool enablePluginMetadataValidation{true};
    bool enablePluginCapabilityValidation{true};
    bool enablePluginSecurityValidation{true};
};

// Plugin Registry
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Disable copying, enable moving
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    PluginRegistry(PluginRegistry&&) noexcept;
    PluginRegistry& operator=(PluginRegistry&&) noexcept;
    
    // Initialize plugin registry
    bool initialize(const PluginRegistryConfig& config = PluginRegistryConfig());
    
    // Shutdown plugin registry
    void shutdown();
    
    // Check if plugin registry is active
    bool isActive() const;
    
    // Plugin registration
    bool registerPlugin(std::unique_ptr<Plugin> plugin);
    bool unregisterPlugin(const std::string& name);
    bool updatePlugin(std::unique_ptr<Plugin> plugin);
    bool forceUnregisterPlugin(const std::string& name); // Bypasses validation for cleanup
    
    // Plugin lookup
    Plugin* getPlugin(const std::string& name);
    const Plugin* getPlugin(const std::string& name) const;
    Plugin* getPluginByCategory(const std::string& category);
    Plugin* getPluginByCapability(const std::string& capability);
    
    // Plugin information
    bool isPluginRegistered(const std::string& name) const;
    bool isPluginActive(const std::string& name) const;
    PluginMetadata getPluginMetadata(const std::string& name) const;
    PluginLoadStatus getPluginStatus(const std::string& name) const;
    PluginCapabilities getPluginCapabilities(const std::string& name) const;
    
    // Plugin listing
    std::vector<Plugin*> getPluginsByCategory(const std::string& category);
    std::vector<Plugin*> getPluginsByCapability(const std::string& capability);
    std::vector<Plugin*> getPluginsByTag(const std::string& tag);
    std::vector<Plugin*> getPluginsBySecurityLevel(PluginSecurityLevel level);
    std::vector<Plugin*> getPluginsByCategoryAndCapability(const std::string& category, 
                                                        const std::string& capability);
    std::vector<Plugin*> getAllPlugins();
    std::vector<std::string> getAllPluginNames() const;
    std::vector<std::string> getPluginsByCategoryNames(const std::string& category) const;
    std::vector<std::string> getPluginsByCapabilityNames(const std::string& capability) const;
    std::vector<std::string> getPluginsByTagNames(const std::string& tag) const;
    
    // Plugin validation and compatibility
    bool validatePlugin(const Plugin& plugin) const;
    bool validatePlugin(const std::string& name) const;
    std::vector<std::string> getPluginValidationErrors(const Plugin& plugin) const;
    std::vector<std::string> getPluginValidationErrors(const std::string& name) const;
    bool checkPluginDependencies(const Plugin& plugin) const;
    bool checkPluginDependencies(const std::string& name) const;
    bool resolvePluginDependencies(const std::string& name) const;
    bool resolveAllPluginDependencies() const;
    std::vector<std::string> getDependencyOrder() const;
    std::vector<std::string> getDependentPlugins(const std::string& name) const;
    std::vector<std::string> getConflictingPlugins(const std::string& name) const;
    
    // Plugin compatibility
    bool arePluginsCompatible(const std::string& name1, const std::string& name2) const;
    std::vector<std::pair<std::string, std::string>> getIncompatiblePluginPairs() const;
    std::vector<std::string> getCompatiblePlugins(const std::string& name) const;
    
    // Plugin metadata management
    bool updatePluginMetadata(const std::string& name, const PluginMetadata& metadata);
    bool addPluginTags(const std::string& name, const std::vector<std::string>& tags);
    bool removePluginTags(const std::string& name, const std::vector<std::string>& tags);
    bool setPluginSecurityLevel(const std::string& name, PluginSecurityLevel level);
    bool addPluginCapability(const std::string& name, const PluginCapabilities& capabilities);
    
    // Plugin version management
    std::string getPluginVersion(const std::string& name) const;
    bool checkPluginVersion(const std::string& name, const std::string& requiredVersion) const;
    bool updatePluginVersion(const std::string& name, const std::string& newVersion);
    std::vector<std::string> getPluginVersionHistory(const std::string& name) const;
    bool addPluginVersion(const std::string& name, const std::string& version);
    
    // Plugin statistics
    PluginStatistics getPluginStatistics(const std::string& name) const;
    PluginSystemStatistics getSystemStatistics() const;
    size_t getPluginCount() const;
    size_t getActivePluginCount() const;
    size_t getLoadedPluginCount() const;
    size_t getFailedPluginCount() const;
    double getSystemMemoryUsage() const;
    double getSystemCPUUsage() const;
    
    // Plugin path and discovery
    bool addPluginPath(const std::filesystem::path& path);
    bool removePluginPath(const std::filesystem::path& path);
    std::vector<std::filesystem::path> getPluginPaths() const;
    std::vector<std::string> discoverPlugins(const std::filesystem::path& directory) const;
    bool loadPluginFromPath(const std::filesystem::path& path);
    bool unloadPluginFromPath(const std::filesystem::path& path);
    
    // Plugin configuration
    bool setPluginConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config);
    std::map<std::string, PluginConfigValue> getPluginConfig(const std::string& name) const;
    std::map<std::string, std::map<std::string, PluginConfigValue>> getAllPluginConfigs() const;
    bool savePluginConfig(const std::string& name, const std::string& filepath) const;
    bool loadPluginConfig(const std::string& name, const std::string& filepath);
    bool resetPluginConfig(const std::string& name);
    bool resetAllPluginConfigs();
    
    // Plugin state management
    bool savePluginState(const std::string& name, const std::string& filepath) const;
    bool loadPluginState(const std::string& name, const std::string& filepath);
    bool saveAllPluginStates(const std::string& directory) const;
    bool loadAllPluginStates(const std::string& directory);
    bool clearPluginState(const std::string& name);
    bool clearAllPluginStates();
    
    // Plugin lifecycle management
    bool startPlugin(const std::string& name);
    bool stopPlugin(const std::string& name);
    bool pausePlugin(const std::string& name);
    bool resumePlugin(const std::string& name);
    bool restartPlugin(const std::string& name);
    bool refreshPlugin(const std::string& name);
    PluginLoadStatus getPluginStatus(const std::string& name);
    std::chrono::system_clock::time_point getPluginStartTime(const std::string& name) const;
    std::chrono::system_clock::time_point getPluginStopTime(const std::string& name) const;
    double getPluginUptime(const std::string& name) const;
    std::map<std::string, PluginLoadStatus> getAllPluginStatuses() const;
    std::map<std::string, std::chrono::system_clock::time_point> getAllPluginStartTimes() const;
    std::map<std::string, std::chrono::system_clock::time_point> getAllPluginStopTimes() const;
    
    // Plugin events
    void addEventCallback(PluginEventCallback callback);
    void removeEventCallback(PluginEventCallback callback);
    void broadcastPluginEvent(const PluginEvent& event);
    
    // Plugin commands
    bool executePluginCommand(const std::string& name, const std::string& command,
                             const std::map<std::string, std::string>& args = {});
    
    // System commands
    bool executeSystemCommand(const std::string& command,
                             const std::map<std::string, std::string>& args = {});
    
    // System maintenance
    bool cleanRegistry();
    bool validateRegistry();
    bool repairRegistry();
    bool optimizeRegistry();
    
    // System information
    std::string getRegistryStatus() const;
    std::string getRegistryInfo() const;
    std::vector<std::string> getRegistryMessages() const;
    std::vector<std::string> getRegistryWarnings() const;
    std::vector<std::string> getRegistryErrors() const;
    
    // System commands
    bool exportRegistry(const std::string& filename, const std::string& format = "json") const;
    bool importRegistry(const std::string& filename, const std::string& format = "json");
    
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
    virtual void onPluginRegistered(const std::string& name);
    virtual void onPluginUnregistered(const std::string& name);
    virtual void onPluginUpdated(const std::string& name);
    virtual void onPluginStarted(const std::string& name);
    virtual void onPluginStopped(const std::string& name);
    virtual void onPluginError(const std::string& name, const std::string& error);
    virtual void onPluginWarning(const std::string& name, const std::string& warning);
    
    // System lifecycle callbacks
    virtual void onRegistryInitialized();
    virtual void onRegistryShutdown();
    virtual void onRegistryError(const std::string& error);
    virtual void onRegistryWarning(const std::string& warning);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool registerPluginInternal(std::unique_ptr<Plugin> plugin);
    bool unregisterPluginInternal(const std::string& name);
    bool updatePluginInternal(const std::string& name, std::unique_ptr<Plugin> plugin);
    bool validatePluginInternal(const Plugin& plugin) const;
    bool checkPluginConflicts(const Plugin& plugin) const;
    bool resolveDependencyGraph() const;
    bool validateAllPlugins() const;
    bool cleanupFailedPlugins() const;
    std::vector<std::string> findPluginsInPath(const std::filesystem::path& path) const;
    bool loadPluginsFromPath(const std::filesystem::path& path);
    bool unloadPluginsFromPath(const std::filesystem::path& path);
    bool savePluginRegistry() const;
    bool loadPluginRegistry();
    bool backupPluginRegistry() const;
    bool restorePluginRegistry() const;
    void logPluginEvent(const PluginEvent& event);
    void updateRegistryStatistics();
    bool cleanupPluginResources(const std::string& name);
    bool validatePluginDependenciesInternal(const std::string& name) const;
    void resolveDependencyOrderInternal();
    std::vector<std::string> getLoadOrderInternal(const std::vector<std::string>& pluginNames) const;
    bool checkVersionCompatibility(const std::string& pluginName, const PluginMetadata& metadata) const;
    bool updatePluginCache(const std::string& name, const PluginMetadata& metadata);
    std::optional<PluginMetadata> getPluginFromCache(const std::string& name) const;
    bool removePluginFromCache(const std::string& name);
    std::vector<std::string> getPluginDependenciesFromCache(const std::string& name) const;
    bool updateDependencyCache(const std::string& name, const std::vector<std::string>& dependencies);
    std::vector<std::string> getConflictingPluginsFromCache(const std::string& name) const;
    bool updateConflictCache(const std::string& name, const std::vector<std::string>& conflicts);
};

} // namespace nlm