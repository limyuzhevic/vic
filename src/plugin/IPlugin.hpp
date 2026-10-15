#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeinfo>

namespace nlm {

// Forward declarations
class Brain;
class Config;

// Plugin interface base class
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin name
    virtual std::string getName() const = 0;
    
    // Get plugin version
    virtual std::string getVersion() const = 0;
    
    // Get plugin description
    virtual std::string getDescription() const = 0;
    
    // Initialize plugin with brain and config
    virtual bool initialize(Brain* brain, std::shared_ptr<Config> config) = 0;
    
    // Update plugin (called every step)
    virtual void update(double timestep) = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Get plugin type
    virtual std::string getType() const = 0;
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<IPlugin>()>;

// Plugin information
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string type;
    std::string filePath;
    bool enabled;
    
    PluginInfo() : enabled(false) {}
};

// Plugin manager interface
class IPluginManager {
public:
    virtual ~IPluginManager() = default;
    
    // Load plugin from file
    virtual bool loadPlugin(const std::string& filePath) = 0;
    
    // Load plugin from factory function
    virtual bool loadPlugin(const std::string& name, PluginFactory factory) = 0;
    
    // Unload plugin
    virtual bool unloadPlugin(const std::string& name) = 0;
    
    // Get plugin by name
    virtual std::unique_ptr<IPlugin> getPlugin(const std::string& name) = 0;
    
    // Get all loaded plugins
    virtual std::vector<std::string> getPluginNames() const = 0;
    
    // Get plugin info
    virtual PluginInfo getPluginInfo(const std::string& name) const = 0;
    
    // Enable/disable plugin
    virtual bool enablePlugin(const std::string& name) = 0;
    virtual bool disablePlugin(const std::string& name) = 0;
    
    // Check if plugin is enabled
    virtual bool isPluginEnabled(const std::string& name) const = 0;
    
    // Initialize all plugins
    virtual void initializePlugins(Brain* brain, std::shared_ptr<Config> config) = 0;
    
    // Update all plugins
    virtual void updatePlugins(double timestep) = 0;
    
    // Shutdown all plugins
    virtual void shutdownPlugins() = 0;
};

// Default plugin manager implementation
class PluginManager : public IPluginManager {
public:
    PluginManager();
    ~PluginManager() override;
    
    // Load plugin from file
    bool loadPlugin(const std::string& filePath) override;
    
    // Load plugin from factory function
    bool loadPlugin(const std::string& name, PluginFactory factory) override;
    
    // Unload plugin
    bool unloadPlugin(const std::string& name) override;
    
    // Get plugin by name
    std::unique_ptr<IPlugin> getPlugin(const std::string& name) override;
    
    // Get all loaded plugins
    std::vector<std::string> getPluginNames() const override;
    
    // Get plugin info
    PluginInfo getPluginInfo(const std::string& name) const override;
    
    // Enable/disable plugin
    bool enablePlugin(const std::string& name) override;
    bool disablePlugin(const std::string& name) override;
    
    // Check if plugin is enabled
    bool isPluginEnabled(const std::string& name) const override;
    
    // Initialize all plugins
    void initializePlugins(Brain* brain, std::shared_ptr<Config> config) override;
    
    // Update all plugins
    void updatePlugins(double timestep) override;
    
    // Shutdown all plugins
    void shutdownPlugins() override;
    
    // Register built-in plugins
    void registerBuiltInPlugins();
    
    // Create plugin registry
    struct PluginRegistry {
        std::string name;
        std::string version;
        std::string description;
        PluginFactory factory;
        std::string type;
        
        PluginRegistry(const std::string& n, const std::string& v, 
                     const std::string& d, PluginFactory f, const std::string& t)
            : name(n), version(v), description(d), factory(f), type(t) {}
    };
    
    std::vector<PluginRegistry> pluginRegistry;
    
    // Plugin storage
    std::unordered_map<std::string, std::unique_ptr<IPlugin>> plugins;
    std::unordered_map<std::string, PluginInfo> pluginInfos;
    std::unordered_map<std::string, bool> pluginEnabled;
    std::unordered_map<std::string, PluginFactory> pluginFactories;
};

}} // namespace nlm