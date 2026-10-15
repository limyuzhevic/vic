#pragma once

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

namespace nlm {

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
    
    // Get plugin type
    virtual std::string getType() const = 0;
    
    // Initialize plugin with brain and config
    virtual bool initialize(void* context, std::shared_ptr<void> config) = 0;
    
    // Update plugin (called every step)
    virtual void update(double timestep) = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
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
    bool enabled = false;
};

// Plugin manager interface
class IPluginManager {
public:
    virtual ~IPluginManager() = default;
    
    // Register built-in plugins
    virtual void registerBuiltInPlugins() = 0;
    
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
    virtual void initializePlugins(void* context, std::shared_ptr<void> config) = 0;
    
    // Update all plugins
    virtual void updatePlugins(double timestep) = 0;
    
    // Shutdown all plugins
    virtual void shutdownPlugins() = 0;
};

}} // namespace nlm