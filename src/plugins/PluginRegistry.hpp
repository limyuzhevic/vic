#pragma once

#include "IPlugin.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace plugins {

// Plugin registry and factory for managing plugin lifecycle and creation
class PluginRegistry {
public:
    // Registry singleton
    static PluginRegistry& getInstance();
    
    // Plugin registration
    template<typename PluginType>
    bool registerPlugin(const std::string& name, const std::string& version = "1.0.0");
    
    bool registerPlugin(const PluginEntry& entry);
    bool unregisterPlugin(const std::string& name);
    bool isPluginRegistered(const std::string& name) const;
    
    // Plugin discovery
    std::vector<std::string> getRegisteredPluginNames() const;
    std::vector<PluginEntry> getPluginEntries() const;
    PluginEntry* getPluginEntry(const std::string& name);
    
    // Plugin instantiation
    std::unique_ptr<IPlugin> createPlugin(const std::string& name);
    std::unique_ptr<IPlugin> createPlugin(const PluginEntry& entry);
    
    // Plugin management
    bool loadPlugin(const std::string& name, const std::map<std::string, std::string>& config = {});
    bool unloadPlugin(const std::string& name);
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    
    // Plugin lookup
    IPlugin* getPlugin(const std::string& name);
    const IPlugin* getPlugin(const std::string& name) const;
    
    // Component plugins lookup
    std::vector<IComponentPlugin*> getComponentPlugins(ComponentType type) const;
    IComponentPlugin* getComponentPlugin(const std::string& name);
    
    // Plasticity plugins lookup
    std::vector<IPlasticityPlugin*> getPlasticityPlugins() const;
    IPlasticityPlugin* getPlasticityPlugin(const std::string& name);
    
    // Neuromodulation plugins lookup
    std::vector<INeuromodulationPlugin*> getNeuromodulationPlugins() const;
    INeuromodulationPlugin* getNeuromodulationPlugin(const std::string& name);
    
    // Memory plugins lookup
    std::vector<IMemoryPlugin*> getMemoryPlugins() const;
    IMemoryPlugin* getMemoryPlugin(const std::string& name);
    
    // Development plugins lookup
    std::vector<IDevelopmentPlugin*> getDevelopmentPlugins() const;
    IDevelopmentPlugin* getDevelopmentPlugin(const std::string& name);
    
    // Plugin statistics
    size_t getPluginCount() const;
    size_t getLoadedPluginCount() const;
    size_t getEnabledPluginCount() const;
    
    // Configuration management
    bool setGlobalConfig(const std::string& key, const std::string& value);
    std::string getGlobalConfig(const std::string& key) const;
    std::map<std::string, std::string> getGlobalConfig() const;
    
    // Event system
    using EventCallback = std::function<void(const std::string& eventName, const std::map<std::string, std::string>& data)>;
    void subscribeToPluginEvent(const std::string& eventName, const EventCallback& callback);
    void unsubscribeFromPluginEvent(const std::string& eventName, const EventCallback& callback);
    
    // Plugin search and filtering
    std::vector<std::string> findPluginsByTag(const std::string& tag) const;
    std::vector<std::string> findPluginsByCategory(const std::string& category) const;
    std::vector<std::string> findPluginsWithDependency(const std::string& dependency) const;
    
    // Health and status
    bool validatePluginDependencies(const std::string& pluginName) const;
    std::vector<PluginError> getPluginErrors(const std::string& pluginName) const;
    bool hasPluginErrors(const std::string& pluginName) const;
    
    // Performance monitoring
    std::map<std::string, double> getPluginPerformanceMetrics(const std::string& pluginName) const;
    std::map<std::string, std::map<std::string, double>> getAllPluginPerformanceMetrics() const;
    
    // Plugin lifecycle management
    void updatePlugins(TimestepDuration dt);
    void shutdownAllPlugins();
    void cleanupAllPlugins();
    
    // Plugin factory methods
    template<typename PluginType>
    void registerPluginFactory(const std::string& name, 
                              std::function<std::unique_ptr<PluginType>()> factory);
    
    // Plugin loading configuration
    void setPluginSearchPath(const std::string& path);
    void addPluginSearchPath(const std::string& path);
    std::vector<std::string> getPluginSearchPaths() const;
    
    // Plugin versioning
    bool updatePluginVersion(const std::string& name, const std::string& version);
    std::string getPluginVersion(const std::string& name) const;
    
    // Backup and restore
    bool backupPluginState(const std::string& name);
    bool restorePluginState(const std::string& name);
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
private:
    PluginRegistry();
    ~PluginRegistry();
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    
    // Internal implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<IPlugin>()>;

// Plugin loader interface
class IPluginLoader {
public:
    virtual ~IPluginLoader() = default;
    virtual std::unique_ptr<IPlugin> loadPlugin(const std::string& name) = 0;
    virtual bool unloadPlugin(IPlugin* plugin) = 0;
    virtual bool isPluginLoaded(const std::string& name) const = 0;
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
};

// Dynamic library plugin loader
class DynamicPluginLoader : public IPluginLoader {
public:
    DynamicPluginLoader();
    ~DynamicPluginLoader() override;
    
    std::unique_ptr<IPlugin> loadPlugin(const std::string& name) override;
    bool unloadPlugin(IPlugin* plugin) override;
    bool isPluginLoaded(const std::string& name) const override;
    std::vector<std::string> getLoadedPlugins() const override;
    
    void setLibraryPath(const std::string& path);
    void addLibraryPath(const std::string& path);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace plugins
} // namespace nlm
