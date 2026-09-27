#ifndef PLUGIN_SYSTEM_HPP
#define PLUGIN_SYSTEM_HPP

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <algorithm>
#include <filesystem>
#include <atomic>

namespace nlm {
namespace integration {

// Plugin metadata and configuration
struct PluginInfo {
    std::string name;                    // Plugin name
    std::string version;                 // Version number
    std::string author;                  // Author/creator
    std::string description;             // Description
    std::string plugin_path;             // Path to plugin file
    std::string interface_version;       // Required interface version
    std::map<std::string, std::string> configuration;  // Plugin configuration
    std::vector<std::string> dependencies; // List of required plugins
};

// Plugin capability interface
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Plugin lifecycle management
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual bool shutdown() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;
    
    // Plugin information
    virtual const PluginInfo& getInfo() const = 0;
    virtual std::string getInterfaceVersion() const = 0;
    
    // Plugin functionality
    virtual std::string getName() const = 0;
    virtual std::vector<std::string> getCapabilities() const = 0;
    
    // Communication and event handling
    virtual bool handleEvent(const std::string& event_type, const std::map<std::string, std::string>& data) = 0;
    virtual bool subscribeToEvent(const std::string& event_type) = 0;
    virtual bool unsubscribeFromEvent(const std::string& event_type) = 0;
    
    // Resource management
    virtual bool allocateResources(size_t resources) = 0;
    virtual bool releaseResources(size_t resources) = 0;
    virtual size_t getAvailableResources() const = 0;
};

// Plugin loader interface
class PluginLoader {
public:
    virtual ~PluginLoader() = default;
    
    // Plugin discovery and loading
    virtual std::vector<PluginInfo> discoverPlugins(const std::string& directory) = 0;
    virtual std::unique_ptr<PluginInterface> loadPlugin(const std::string& plugin_path) = 0;
    
    // Plugin validation
    virtual bool validatePlugin(const std::string& plugin_path) = 0;
    virtual std::string getPluginError() const = 0;
    
    // Resource management
    virtual bool unloadPlugin(const std::string& plugin_name) = 0;
    virtual bool cleanupUnloadedPlugins() = 0;
};

// Plugin manager
class PluginManager {
private:
    std::map<std::string, std::unique_ptr<PluginInterface>> loaded_plugins_;
    std::map<std::string, PluginInfo> plugin_infos_;
    std::map<std::string, std::vector<std::string>> plugin_dependencies_;
    std::mutex mutex_;
    std::atomic<bool> plugins_initialized_;
    std::map<std::string, std::vector<std::string>> event_subscriptions_;

public:
    PluginManager() : plugins_initialized_(false) {}

    // Plugin management
    bool loadPlugin(const std::string& plugin_path);
    bool unloadPlugin(const std::string& plugin_name);
    bool reloadPlugin(const std::string& plugin_name);
    
    // Plugin discovery
    std::vector<PluginInfo> discoverPlugins(const std::string& directory);
    PluginInfo getPluginInfo(const std::string& plugin_name) const;
    
    // Plugin lifecycle
    bool initializeAllPlugins();
    bool shutdownAllPlugins();
    bool pauseAllPlugins();
    bool resumeAllPlugins();
    
    // Plugin queries
    std::vector<std::string> getLoadedPluginNames() const;
    bool isPluginLoaded(const std::string& plugin_name) const;
    bool isPluginInitialized(const std::string& plugin_name) const;
    
    // Event handling
    bool publishEvent(const std::string& event_type, const std::map<std::string, std::string>& data);
    bool subscribeToEvent(const std::string& plugin_name, const std::string& event_type);
    bool unsubscribeFromEvent(const std::string& plugin_name, const std::string& event_type);
    
    // Resource management
    bool allocateResources(const std::string& plugin_name, size_t resources);
    bool releaseResources(const std::string& plugin_name, size_t resources);
    std::map<std::string, size_t> getResourceUsage() const;
    
    // Plugin configuration
    bool configurePlugin(const std::string& plugin_name, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfiguration(const std::string& plugin_name) const;
    
    // Plugin dependencies
    bool addPluginDependency(const std::string& plugin_name, const std::string& dependency);
    std::vector<std::string> getPluginDependencies(const std::string& plugin_name) const;
    bool checkPluginDependencies(const std::string& plugin_name) const;
    
    // Persistence
    bool savePluginState(const std::string& filename) const;
    bool loadPluginState(const std::string& filename);
    
    // Validation
    bool validatePluginConfiguration(const std::string& plugin_name) const;
    std::vector<std::string> getValidationErrors() const;
};

// Plugin system interface
class PluginSystem {
private:
    std::unique_ptr<PluginManager> plugin_manager_;
    std::map<std::string, std::function<void(const std::map<std::string, std::string>&)>> event_handlers_;
    bool enabled_;

public:
    PluginSystem() : enabled_(false) {
        plugin_manager_ = std::make_unique<PluginManager>();
    }

    // System control
    bool enable() { enabled_ = true; return true; }
    bool disable() { enabled_ = false; return true; }
    bool isEnabled() const { return enabled_; }
    
    // Plugin system management
    PluginManager& getPluginManager() { return *plugin_manager_; }
    const PluginManager& getPluginManager() const { return *plugin_manager_; }
    
    // Event handling
    bool registerEventHandler(const std::string& event_type, 
                             std::function<void(const std::map<std::string, std::string>&)> handler);
    bool unregisterEventHandler(const std::string& event_type);
    bool handleSystemEvent(const std::string& event_type, const std::map<std::string, std::string>& data);
    
    // Integration with core NLM
    bool integrateWithBrain(const std::string& brain_name);
    bool integrateWithSimulation(const std::string& simulation_name);
    bool integrateWithAnalysis(const std::string& analysis_name);
    
    // Configuration
    bool configure(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getConfiguration() const;
    
    // Monitoring and statistics
    std::map<std::string, size_t> getPluginStatistics() const;
    double getSystemHealth() const;
    std::string getSystemStatus() const;
    
    // Persistence and backup
    bool savePluginState(const std::string& filename) const;
    bool loadPluginState(const std::string& filename);
    bool backupPluginState(const std::string& backup_name) const;
    bool restorePluginState(const std::string& backup_name);
    
    // Plugin discovery and loading
    std::vector<PluginInfo> discoverPlugins(const std::string& directory);
    bool loadPluginFromPath(const std::string& plugin_path);
    bool unloadPlugin(const std::string& plugin_name);
    
    // Plugin lifecycle
    bool initializePlugins();
    bool shutdownPlugins();
    bool pausePlugins();
    bool resumePlugins();
    
    // Plugin information
    std::vector<PluginInfo> getLoadedPlugins() const;
    PluginInfo getPluginInfo(const std::string& plugin_name) const;
    bool validatePlugin(const std::string& plugin_name) const;
};

} // namespace integration
} // namespace nlm

#endif // PLUGIN_SYSTEM_HPP