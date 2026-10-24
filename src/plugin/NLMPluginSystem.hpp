#pragma once

/**
 * Plugin System for NLM
 * 
 * This provides a plugin architecture for extending the NLM brain system
 * with custom functionality. Plugins can add new neural dynamics,
 * plasticity rules, memory systems, cognition modules, or neuromodulation
 * effects.
 */

#include "brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

/**
 * Base class for all NLM plugins
 */
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    /**
     * Get the plugin name
     */
    virtual std::string getName() const = 0;
    
    /**
     * Get the plugin version
     */
    virtual std::string getVersion() const = 0;
    
    /**
     * Initialize the plugin
     */
    virtual bool initialize(Brain* brain) {
        brain_ = brain;
        return true;
    }
    
    /**
     * Process a simulation step
     */
    virtual void processStep(SimulationStep step, Timestamp time) {}
    
    /**
     * Clean up the plugin
     */
    virtual void shutdown() {}
    
    /**
     * Get the brain instance
     */
    Brain* getBrain() const { return brain_; }

protected:
    Brain* brain_ = nullptr;
};

/**
 * Plugin registry for managing NLM plugins
 */
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    /**
     * Load a plugin from a shared library
     * 
     * @param pluginPath Path to the plugin shared library
     * @return true if plugin loaded successfully
     */
    bool loadPlugin(const std::string& pluginPath);
    
    /**
     * Unload a plugin
     * 
     * @param pluginName Name of the plugin to unload
     * @return true if plugin unloaded successfully
     */
    bool unloadPlugin(const std::string& pluginName);
    
    /**
     * Get a plugin by name
     * 
     * @param pluginName Name of the plugin
     * @return Plugin instance or nullptr if not found
     */
    std::shared_ptr<NLMPlugin> getPlugin(const std::string& pluginName);
    
    /**
     * Get all loaded plugins
     */
    std::vector<std::shared_ptr<NLMPlugin>> getAllPlugins() const;
    
    /**
     * Initialize all plugins
     */
    bool initializeAllPlugins(Brain* brain);
    
    /**
     * Process a simulation step for all plugins
     */
    void processStepAllPlugins(SimulationStep step, Timestamp time);
    
    /**
     * Shutdown all plugins
     */
    void shutdownAllPlugins();
    
    /**
     * Check if a plugin is loaded
     * 
     * @param pluginName Name of the plugin
     * @return true if plugin is loaded
     */
    bool isPluginLoaded(const std::string& pluginName) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Plugin factory for creating plugin instances
 */
class PluginFactory {
public:
    PluginFactory();
    ~PluginFactory();
    
    /**
     * Register a plugin factory function
     * 
     * @param pluginName Name of the plugin
     * @param factory Function that creates plugin instances
     */
    void registerPlugin(const std::string& pluginName, 
                       std::function<std::shared_ptr<NLMPlugin>()> factory);
    
    /**
     * Create a plugin instance
     * 
     * @param pluginName Name of the plugin to create
     * @return Plugin instance or nullptr if not found
     */
    std::shared_ptr<NLMPlugin> createPlugin(const std::string& pluginName);
    
    /**
     * Get list of registered plugin names
     */
    std::vector<std::string> getRegisteredPlugins() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Built-in plugin examples
 */
namespace BuiltInPlugins {
    /**
     * Create a memory enhancement plugin
     * 
     * This plugin adds enhanced memory capabilities to the brain
     */
    std::shared_ptr<NLMPlugin> createMemoryEnhancementPlugin();
    
    /**
     * Create a prediction enhancement plugin
     * 
     * This plugin adds enhanced prediction capabilities to the brain
     */
    std::shared_ptr<NLMPlugin> createPredictionEnhancementPlugin();
    
    /**
     * Create a cognition enhancement plugin
     * 
     * This plugin adds enhanced cognition capabilities to the brain
     */
    std::shared_ptr<NLMPlugin> createCognitionEnhancementPlugin();
    
    /**
     * Create a neuromodulation enhancement plugin
     * 
     * This plugin adds enhanced neuromodulation capabilities to the brain
     */
    std::shared_ptr<NLMPlugin> createNeuromodulationEnhancementPlugin();
}

} // namespace nlm