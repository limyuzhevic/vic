// Plugin architecture for NLM cognitive extension system
// 
// This provides a framework for extending the NLM system with new mechanisms
// without modifying the core codebase. Supports plugin registration,
// discovery, and factory-based instantiation.

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <iostream>

namespace nlm {

// Plugin interface - base class for all plugins
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Get plugin name (unique identifier)
    virtual std::string getName() const = 0;
    
    // Get plugin version
    virtual std::string getVersion() const = 0;
    
    // Get plugin description
    virtual std::string getDescription() const = 0;
    
    // Initialize plugin (called when plugin is loaded)
    virtual bool initialize() {
        return true;
    }
    
    // Cleanup plugin (called when plugin is unloaded)
    virtual void cleanup() {}
    
    // Check if plugin is enabled
    virtual bool isEnabled() const {
        return true;
    }
    
    // Set plugin enabled state
    virtual void setEnabled(bool enabled) {}
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<Plugin>()>;

// Plugin registry for managing plugins
class PluginRegistry {
public:
    PluginRegistry() = default;
    
    // Register a plugin factory
    void registerPlugin(const std::string& name, 
                       const std::string& version,
                       const std::string& description,
                       PluginFactory factory);
    
    // Get plugin factory by name
    PluginFactory getPlugin(const std::string& name) const;
    
    // Check if plugin is registered
    bool isRegistered(const std::string& name) const;
    
    // List all registered plugin names
    std::vector<std::string> getRegisteredPluginNames() const;
    
    // Load and instantiate a plugin
    std::unique_ptr<Plugin> createPlugin(const std::string& name);
    
    // Load all plugins from directory (platform-specific implementation)
    bool loadPluginsFromDirectory(const std::string& directory);
    
    // Get plugin information
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        bool enabled;
    };
    
    std::vector<PluginInfo> getPluginInfo() const;
    
    // Enable/disable plugin
    void setPluginEnabled(const std::string& name, bool enabled);
    
    // Cleanup all plugins
    void cleanupAllPlugins();
    
private:
    std::map<std::string, std::pair<std::string, std::string>> pluginMetadata;
    std::map<std::string, PluginFactory> pluginFactories;
    std::map<std::string, std::unique_ptr<Plugin>> pluginInstances;
    std::map<std::string, bool> pluginEnabled;
};

// Global plugin registry instance
inline PluginRegistry& getGlobalPluginRegistry() {
    static PluginRegistry registry;
    return registry;
}

// Convenience macros for plugin registration
#define REGISTER_PLUGIN(name, version, description, factory) \
namespace { \
    struct name##_registration { \
        name##_registration() { \
            getGlobalPluginRegistry().registerPlugin(name, version, description, factory); \
        } \
    } name##_reg; \
}

// Plugin base classes for specific mechanism types
class PlasticityPlugin : public Plugin {
public:
    virtual ~PlasticityPlugin() = default;
    
    // Get plasticity name
    virtual std::string getPlasticityName() const = 0;
    
    // Create a plasticity rule instance
    virtual std::unique_ptr<class PlasticityRule> createPlasticityRule() = 0;
};

class NeuromodulationPlugin : public Plugin {
public:
    virtual ~NeuromodulationPlugin() = default;
    
    // Get neuromodulation name
    virtual std::string getNeuromodulationName() const = 0;
    
    // Create a neuromodulation instance
    virtual std::unique_ptr<class Neuromodulator> createNeuromodulator() = 0;
};

class MemoryPlugin : public Plugin {
public:
    virtual ~MemoryPlugin() = default;
    
    // Get memory system name
    virtual std::string getMemorySystemName() const = 0;
    
    // Create a memory system instance
    virtual std::unique_ptr<class MemorySystem> createMemorySystem() = 0;
};

// Plugin utilities
namespace PluginUtils {
    // Plugin loading utilities
    std::string findPluginDirectory();
    std::string getPlatformPluginExtension();
    
    // Plugin validation
    bool validatePluginName(const std::string& name);
    bool validatePluginVersion(const std::string& version);
    
    // Logging
    void logPluginEvent(const std::string& pluginName, const std::string& event);
}

} // namespace nlm