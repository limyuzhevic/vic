#ifndef NLM_PLUGIN_PLUGINMANAGER_HPP
#define NLM_PLUGIN_PLUGINMANAGER_HPP

#include "PluginBase.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <functional>
#include <thread>
#include <future>

namespace nlm {
namespace plugin {

class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin registration
    template<typename TPlugin, typename... Args>
    bool registerPlugin(const std::string& name, Args&&... args);
    
    template<typename TPlugin>
    bool registerPlugin(std::unique_ptr<TPlugin> plugin);
    
    template<typename TPlugin>
    std::shared_ptr<TPlugin> getPlugin(const std::string& name);
    
    // Plugin loading from shared libraries
    bool loadPlugin(const std::string& name, const std::string& path);
    bool unloadPlugin(const std::string& name);
    
    // Plugin lifecycle management
    bool initializePlugin(const std::string& name);
    bool initializeAllPlugins();
    bool activatePlugin(const std::string& name);
    bool activateAllPlugins();
    bool shutdownPlugin(const std::string& name);
    bool shutdownAllPlugins();
    bool unloadAllPlugins();
    
    // Plugin discovery and information
    std::vector<std::string> getAvailablePlugins() const;
    PluginInfo getPluginInfo(const std::string& name) const;
    PluginMetadata getPluginMetadata(const std::string& name) const;
    PluginState getPluginState(const std::string& name) const;
    
    // Plugin configuration
    bool setPluginConfig(const std::string& name, const std::unordered_map<std::string, std::string>& config);
    std::unordered_map<std::string, std::string> getPluginConfig(const std::string& name) const;
    
    // Plugin dependency management
    bool addPluginDependency(const std::string& plugin, const std::string& dependency);
    bool removePluginDependency(const std::string& plugin, const std::string& dependency);
    std::vector<std::string> getPluginDependencies(const std::string& name) const;
    
    // Plugin conflict detection and resolution
    bool checkAllConflicts();
    bool resolveConflicts();
    std::vector<std::pair<std::string, std::string>> getConflicts() const;
    
    // Plugin extension access
    template<typename TExtension>
    std::shared_ptr<TExtension> getPluginExtension(const std::string& pluginName, const std::string& extensionName);
    
    // Callback functions
    using PluginLoadCallback = std::function<void(const std::string& name, bool success)>;
    using PluginStateChangeCallback = std::function<void(const std::string& name, PluginState oldState, PluginState newState)>;
    
    void addLoadCallback(const PluginLoadCallback& callback);
    void addStateChangeCallback(const PluginStateChangeCallback& callback);
    
    // Plugin statistics and monitoring
    struct PluginStats {
        std::string name;
        PluginState state;
        std::chrono::system_clock::time_point loadTime;
        std::chrono::system_clock::time_point lastActivity;
        uint64_t loadCount = 0;
        uint64_t activateCount = 0;
        uint64_t shutdownCount = 0;
        uint64_t errorCount = 0;
        std::string lastError;
    };
    
    std::vector<PluginStats> getAllPluginStats() const;
    
    // Thread safety
    bool isThreadSafe() const { return threadSafe_; }
    void setThreadSafe(bool enabled) { threadSafe_ = enabled; }
    
private:
    struct PluginEntry {
        std::shared_ptr<IPlugin> plugin;
        PluginMetadata metadata;
        std::unordered_map<std::string, std::string> config;
        PluginState state;
        std::string error;
        std::chrono::system_clock::time_point loadTime;
        std::unordered_map<std::string, std::string> dependencies;
        std::vector<std::string> conflictingPlugins;
    };
    
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, PluginEntry> plugins_;
    bool threadSafe_;
    
    std::vector<PluginLoadCallback> loadCallbacks_;
    std::vector<PluginStateChangeCallback> stateChangeCallbacks_;
    
    void notifyLoadCallbacks(const std::string& name, bool success);
    void notifyStateChangeCallbacks(const std::string& name, PluginState oldState, PluginState newState);
    
    bool validatePlugin(const PluginEntry& entry) const;
    bool initializeDependencies(const std::string& name);
    bool checkAndResolveConflicts(const std::string& name);
};

// PluginLoader for dynamic library loading
class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();
    
    bool loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& path);
    std::string getPluginName(const std::string& path) const;
    PluginVersion getPluginVersion(const std::string& path) const;
    std::vector<std::string> getDependencies(const std::string& path) const;
    
    std::string getPluginExtension() const;
    
    using PluginCreateFunction = IPlugin*(*)(void);
    
private:
    std::unordered_map<std::string, void*> loadedLibraries_;
};

// PluginRegistry for plugin metadata and discovery
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    bool registerPluginMetadata(const PluginMetadata& metadata);
    bool unregisterPluginMetadata(const std::string& name);
    PluginMetadata getPluginMetadata(const std::string& name) const;
    bool updatePluginMetadata(const PluginMetadata& metadata);
    
    std::vector<PluginMetadata> getAllPlugins() const;
    std::vector<PluginMetadata> getPluginsByType(PluginType type) const;
    std::vector<PluginMetadata> getPluginsByAuthor(const std::string& author) const;
    
    bool addPluginSearchPath(const std::string& path);
    std::vector<std::string> discoverPlugins() const;
    
    // Plugin compatibility checking
    bool checkVersionCompatibility(const PluginVersion& pluginVersion, const PluginVersion& requiredVersion) const;
    bool checkDependencies(const PluginMetadata& metadata, const std::unordered_map<std::string, PluginMetadata>& availablePlugins) const;
    
private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, PluginMetadata> metadata_;
    std::vector<std::string> searchPaths_;
};

// PluginInfo struct for public API
struct PluginInfo {
    std::string name;
    PluginVersion version;
    PluginType type;
    std::string description;
    PluginState state;
    std::string author;
    std::string license;
    std::chrono::system_clock::time_point creationTime;
    std::string lastError;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINMANAGER_HPP