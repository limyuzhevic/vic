#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <unordered_set>
#include "PluginBase.hpp"

namespace nlm {
namespace plugin {

// Forward declarations
class DynamicLibraryLoader;

// Plugin registry entry
struct PluginRegistryEntry {
    std::string path;                    // File path to the plugin
    std::string version;                 // Plugin version
    std::string category;                // Plugin category
    std::vector<std::string> dependencies; // Plugin dependencies
    nlohmann::json config;               // Plugin configuration
    std::chrono::system_clock::time_point lastModified; // Last modification time
    bool enabled;                        // Whether plugin is enabled
    bool autoLoad;                       // Whether plugin should be auto-loaded
    
    PluginRegistryEntry()
        : lastModified(std::chrono::system_clock::now())
        , enabled(true)
        , autoLoad(false) {}
};

// Plugin instance management
struct PluginInstance {
    std::string name;                    // Plugin name
    std::unique_ptr<Plugin> plugin;      // Plugin instance
    std::atomic<PluginStatus> status;    // Plugin status
    std::string libraryPath;             // Path to loaded library
    std::shared_ptr<DynamicLibraryLoader> loader; // Library loader
    PluginRegistryEntry registryInfo;    // Registry information
    std::chrono::system_clock::time_point loadTime; // When plugin was loaded
    std::chrono::system_clock::time_point lastAccess; // Last access time
    
    PluginInstance() : status(PluginStatus::UNLOADED) {}
};

// Plugin manager configuration
struct PluginManagerConfig {
    std::string pluginDirectory;         // Directory to search for plugins
    std::vector<std::string> pluginPaths; // Specific plugin file paths
    std::vector<std::string> disabledPlugins; // Plugins to exclude
    bool enableAutoLoad;                 // Automatically load enabled plugins
    bool enableAutoStart;                // Start loaded plugins
    size_t maxConcurrentLoads;           // Maximum concurrent plugin loads
    std::chrono::seconds configCheckInterval; // Interval to check for config changes
    bool threadSafe;                     // Enable thread safety
    std::string pluginNamingConvention;  // Naming convention for plugin files
    
    PluginManagerConfig()
        : enableAutoLoad(true)
        , enableAutoStart(true)
        , maxConcurrentLoads(4)
        , configCheckInterval(std::chrono::seconds(30))
        , threadSafe(true)
        , pluginNamingConvention("*.so") {} // Default to shared libraries
};

// Plugin manager class
class PluginManager {
private:
    PluginManagerConfig config_;
    
    // Plugin storage
    std::unordered_map<std::string, std::unique_ptr<PluginInstance>> plugins_;
    std::map<std::string, std::vector<std::string>> dependencyGraph_;
    
    // Thread safety
    mutable std::mutex pluginsMutex_;
    mutable std::mutex dependencyMutex_;
    std::mutex loaderMutex_;
    std::condition_variable loadCondition_;
    std::atomic<size_t> activeLoadCount_;
    
    // Event handling
    std::function<void(const std::string&)> onPluginLoaded_;
    std::function<void(const std::string&)> onPluginUnloaded_;
    std::function<void(const std::string&, PluginStatus)> onPluginStatusChanged_;
    std::function<void(const std::string&, const std::string&)> onPluginError_;
    
    // Lifecycle management
    std::atomic<bool> running_;
    std::thread configWatcherThread_;
    std::chrono::system_clock::time_point lastConfigCheck_;
    
    // Configuration system integration
    std::unique_ptr<class nlm::Config> globalConfig_;
    
public:
    PluginManager(const PluginManagerConfig& config = PluginManagerConfig());
    ~PluginManager();
    
    // Disable copying and moving
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) = delete;
    PluginManager& operator=(PluginManager&&) = delete;
    
    // Plugin loading/unloading
    bool loadPlugin(const std::string& name);
    bool loadPlugin(const std::string& name, const std::string& path);
    bool loadPlugin(const std::string& name, const PluginRegistryEntry& registryEntry);
    bool unloadPlugin(const std::string& name);
    bool unloadAllPlugins();
    
    // Plugin management
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool isPluginEnabled(const std::string& name) const;
    PluginStatus getPluginStatus(const std::string& name) const;
    
    // Plugin instance access
    Plugin* getPlugin(const std::string& name) const;
    const PluginRegistryEntry& getPluginRegistryInfo(const std::string& name) const;
    std::vector<std::string> getPluginNames() const;
    std::vector<std::string> getLoadedPluginNames() const;
    std::vector<std::string> getPluginNamesByCategory(const std::string& category) const;
    
    // Plugin discovery and registry
    bool scanPluginDirectory();
    bool loadPluginsFromRegistry(const std::string& registryFile);
    bool savePluginsToRegistry(const std::string& registryFile) const;
    bool updatePluginRegistry();
    
    // Configuration integration
    bool loadPluginsFromConfig();
    bool updatePluginConfig(const std::string& name, const nlohmann::json& config);
    nlohmann::json getPluginConfig(const std::string& name) const;
    nlohmann::json getAllPluginConfigs() const;
    
    // Dependency management
    bool resolveDependencies(const std::string& pluginName);
    std::vector<std::string> getPluginDependencies(const std::string& name) const;
    std::vector<std::string> getDependentPlugins(const std::string& name) const;
    bool isDependencySatisfied(const std::string& pluginName) const;
    
    // Plugin lifecycle control
    bool startPlugin(const std::string& name);
    bool stopPlugin(const std::string& name);
    bool restartPlugin(const std::string& name);
    bool startAllPlugins();
    bool stopAllPlugins();
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Event handling
    void setPluginLoadedCallback(std::function<void(const std::string&)> callback);
    void setPluginUnloadedCallback(std::function<void(const std::string&)> callback);
    void setPluginStatusChangedCallback(std::function<void(const std::string&, PluginStatus)> callback);
    void setPluginErrorCallback(std::function<void(const std::string&, const std::string&)> callback);
    
    // Utility functions
    bool pluginExists(const std::string& name) const;
    bool isPluginLoaded(const std::string& name) const;
    nlohmann::json getPluginInfo(const std::string& name) const;
    std::vector<nlohmann::json> getAllPluginInfos() const;
    
    // Configuration management
    void setGlobalConfig(std::unique_ptr<nlm::Config> config);
    std::unique_ptr<nlm::Config>& getGlobalConfig();
    
    // Thread management
    bool start()';
    bool stop();
    bool isRunning() const;
    
    // Error reporting
    std::string getLastError(const std::string& pluginName = "") const;
    std::vector<std::string> getAllErrors() const;
    
    // Statistics and monitoring
    size_t getLoadedPluginCount() const;
    size_t getTotalPluginCount() const;
    float getPluginLoadProgress() const;
    nlohmann::json getManagerStats() const;
    
private:
    // Internal helper methods
    bool loadPluginInternal(const std::string& name, const std::string& path);
    bool createPluginInstance(const std::string& name, const std::string& path);
    bool initializePlugin(const std::string& name);
    bool startPluginInternal(const std::string& name);
    bool stopPluginInternal(const std::string& name);
    bool shutdownPlugin(const std::string& name);
    bool unloadPluginInternal(const std::string& name);
    
    bool scanDirectory(const std::string& directory, std::vector<std::string>& discoveredPlugins);
    bool loadPluginFromFile(const std::string& filepath, PluginRegistryEntry& registryEntry);
    std::string generatePluginNameFromPath(const std::string& filepath) const;
    
    bool validatePlugin(const Plugin* plugin) const;
    bool checkDependencies(const std::string& pluginName) const;
    void triggerEventHandlers(const std::string& pluginName, PluginStatus status, const std::string& error = "");
    
    void configWatcherLoop();
    void cleanupInactivePlugins();
    
    // Event callbacks
    void onPluginLoadedInternal(const std::string& name);
    void onPluginUnloadedInternal(const std::string& name);
    void onPluginStatusChangedInternal(const std::string& name, PluginStatus status);
    void onPluginErrorInternal(const std::string& name, const std::string& error);
    
    // Statistics
    mutable std::chrono::system_clock::time_point startTime_;
    size_t totalLoadAttempts_;
    size_t successfulLoads_;
    size_t failedLoads_;
    mutable std::vector<std::string> allErrors_;
};

}}} // namespace nlm::plugin