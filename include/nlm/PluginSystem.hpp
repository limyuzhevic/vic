// PluginSystem.hpp - Dynamic plugin loading and management
#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include "dlfcn.h"

namespace nlm {

class Plugin {
public:
    virtual ~Plugin() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
};

class PluginSystem {
public:
    PluginSystem();
    ~PluginSystem();
    
    // Load a plugin from file
    bool loadPlugin(const std::string& pluginPath);
    
    // Unload a plugin
    bool unloadPlugin(const std::string& pluginName);
    
    // Get loaded plugins
    std::vector<std::string> getLoadedPlugins() const;
    
    // Get plugin by name
    std::shared_ptr<Plugin> getPlugin(const std::string& pluginName);
    
    // Execute plugin command
    bool executeCommand(const std::string& pluginName, const std::string& command,
                       const std::string& args = "");
    
    // Register plugin factory functions
    template<typename T>
    void registerPluginFactory(const std::string& name,
                              std::function<std::shared_ptr<T>()> factory) {
        factoryRegistry_[name] = [factory]() { return std::shared_ptr<Plugin>(factory().release()); };
    }
    
    // Check if plugin is loaded
    bool isPluginLoaded(const std::string& pluginName) const;
    
    // Clear all plugins
    void clear();
    
    // Get system information
    std::string getSystemInfo() const;
    
private:
    struct PluginHandle {
        void* handle = nullptr;
        std::shared_ptr<Plugin> plugin = nullptr;
        std::string path;
    };
    
    std::unordered_map<std::string, PluginHandle> plugins_;
    std::unordered_map<std::string, std::function<std::shared_ptr<Plugin>()>> factoryRegistry_;
    
    static std::string getPluginError();
};

// Plugin interface base classes for specific component types
class BrainRegionPlugin : public Plugin {
public:
    virtual ~BrainRegionPlugin() = default;
    virtual std::string getRegionType() const = 0;
    virtual std::shared_ptr<void> createRegion() = 0;
};

class PlasticityRulePlugin : public Plugin {
public:
    virtual ~PlasticityRulePlugin() = default;
    virtual std::string getRuleType() const = 0;
    virtual std::shared_ptr<void> createRule() = 0;
};

class MemorySystemPlugin : public Plugin {
public:
    virtual ~MemorySystemPlugin() = default;
    virtual std::string getSystemType() const = 0;
    virtual std::shared_ptr<void> createSystem() = 0;
};

class NeuronTypePlugin : public Plugin {
public:
    virtual ~NeuronTypePlugin() = default;
    virtual std::string getNeuronType() const = 0;
    virtual std::shared_ptr<void> createNeuron() = 0;
};

// Convenience functions
class PluginManager {
public:
    static PluginManager& getInstance();
    
    // Initialize plugin system
    bool initialize();
    
    // Shutdown plugin system
    void shutdown();
    
    // Load all plugins from directory
    bool loadPluginsFromDirectory(const std::string& directoryPath);
    
    // Get plugin system instance
    PluginSystem& getPluginSystem();
    
    // Plugin lifecycle management
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    bool reloadPlugin(const std::string& pluginName);
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directoryPath);
    
    // Plugin information
    std::string getPluginInfo(const std::string& pluginName);
    std::vector<std::string> getPluginDependencies(const std::string& pluginName);
    
private:
    PluginManager() = default;
    ~PluginManager() = default;
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    std::unique_ptr<PluginSystem> pluginSystem_;
    bool initialized_ = false;
};
}