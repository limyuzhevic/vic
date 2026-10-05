#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class Config;

/**
 * Base interface for plugin modules that can be integrated into the brain
 */
class IPluginModule {
public:
    virtual ~IPluginModule() = default;
    
    /**
     * Initialize the module with reference to the brain and configuration
     */
    virtual bool initialize(Brain* brain, std::shared_ptr<Config> config) = 0;
    
    /**
     * Update the module with current simulation timestep
     */
    virtual void update(double timestep) = 0;
    
    /**
     * Get the module's name for identification and logging
     */
    virtual std::string getName() const = 0;
    
    /**
     * Check if this module is enabled based on configuration
     */
    virtual bool isEnabled() const = 0;
};

/**
 * Plugin registry for managing cognitive modules
 */
class PluginRegistry {
public:
    PluginRegistry() = default;
    
    /**
     * Register a plugin module factory
     */
    void registerPlugin(const std::string& name, 
                       std::function<std::unique_ptr<IPluginModule>()> factory,
                       const std::string& description = "",
                       bool enabled = true);
    
    /**
     * Create a plugin instance
     */
    std::unique_ptr<IPluginModule> createPlugin(const std::string& name, 
                                               Brain* brain,
                                               std::shared_ptr<Config> config);
    
    /**
     * Get all registered plugin names
     */
    std::vector<std::string> getRegisteredPluginNames() const;
    
    /**
     * Get plugin description
     */
    std::string getPluginDescription(const std::string& name) const;
    
    /**
     * Enable or disable a plugin
     */
    void setPluginEnabled(const std::string& name, bool enabled);
    
    /**
     * Check if a plugin is enabled
     */
    bool isPluginEnabled(const std::string& name) const;
    
    /**
     * Load plugins from configuration file
     */
    void loadFromConfig(const Config& config);
    
    /**
     * Update all active plugins
     */
    void updateAll(double timestep);
    
    /**
     * Initialize all plugins
     */
    bool initializeAll(Brain* brain, std::shared_ptr<Config> config);

private:
    struct PluginInfo {
        std::function<std::unique_ptr<IPluginModule>()> factory;
        std::string description;
        bool enabled;
    };
    
    std::unordered_map<std::string, PluginInfo> plugins_;
};

/**
 * Brain interface extension for plugin support
 */
class IPluginCapableBrain {
public:
    virtual ~IPluginCapableBrain() = default;
    
    /**
     * Get the plugin registry for this brain
     */
    virtual PluginRegistry& getPluginRegistry() = 0;
    
    /**
     * Check if plugin system is supported
     */
    virtual bool supportsPlugins() const = 0;
};

} // namespace nlm