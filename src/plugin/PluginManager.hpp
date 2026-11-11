#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <map>
#include <any>
#include <filesystem>
#include <mutex>
#include <algorithm>
#include <type_traits>

namespace nlm {

/**
 * Base class for all NLM plugins
 * 
 * Plugins extend NLM functionality by implementing specific interfaces
 * for different components of the system (brain, memory, plasticity,
 * neuromodulation, etc.).
 */
class Plugin {
public:
    virtual ~Plugin() = default;

    /**
     * Get the plugin name
     * 
     * @return Plugin name
     */
    virtual std::string getName() const = 0;

    /**
     * Get the plugin version
     * 
     * @return Plugin version
     */
    virtual std::string getVersion() const = 0;

    /**
     * Get the plugin author
     * 
     * @return Plugin author
     */
    virtual std::string getAuthor() const = 0;

    /**
     * Get the plugin description
     * 
     * @return Plugin description
     */
    virtual std::string getDescription() const = 0;

    /**
     * Get the plugin handle (for dynamic library loading)
     * 
     * @return Plugin handle
     */
    virtual void* getPluginHandle() const { return nullptr; }

    /**
     * Set the plugin handle
     * 
     * @param handle Plugin handle
     */
    virtual void setPluginHandle(void* handle) {}

    /**
     * Get the plugin configuration
     * 
     * @return Plugin configuration
     */
    virtual std::any getPluginConfig() const { return std::any(); }

    /**
     * Set the plugin configuration
     * 
     * @param config Plugin configuration
     */
    virtual void setPluginConfig(const std::any& config) {}

    /**
     * Get the plugin metadata
     * 
     * @return Plugin metadata
     */
    virtual std::unordered_map<std::string, std::string> getPluginMetadata() const { return std::unordered_map<std::string, std::string>(); }

    /**
     * Set the plugin metadata
     * 
     * @param metadata Plugin metadata
     */
    virtual void setPluginMetadata(const std::unordered_map<std::string, std::string>& metadata) {}

protected:
    bool enabled = true;
};

/**
 * Brain plugin interface
 * 
 * Extends brain functionality by adding new neuron types,
 * plasticity rules, or other brain-level features.
 */
class BrainPlugin : public Plugin {
public:
    /**
     * Get the plugin type
     * 
     * @return Plugin type
     */
    virtual std::string getType() const { return "brain"; }

    /**
     * Apply plugin effects to a brain
     * 
     * @param brain Brain to modify
     * @param timestep Current simulation timestep
     * @param currentTime Current simulation time
     */
    virtual void applyToBrain(class Brain* brain, SimulationStep timestep, Timestamp currentTime) {}

    /**
     * Get additional neuron types provided by the plugin
     * 
     * @return Vector of additional neuron types
     */
    virtual std::vector<NeuronType> getAdditionalNeuronTypes() const { return {}; }

    /**
     * Get additional plasticity rules provided by the plugin
     * 
     * @return Vector of additional plasticity rules
     */
    virtual std::vector<PlasticityFlags> getAdditionalPlasticityFlags() const { return {}; }

    /**
     * Get additional neuromodulators provided by the plugin
     * 
     * @return Vector of additional neuromodulators
     */
    virtual std::vector<std::string> getAdditionalNeuromodulators() const { return {}; }
};

/**
 * Memory plugin interface
 * 
 * Extends memory functionality by adding new memory systems,
 * consolidation algorithms, or memory access patterns.
 */
class MemoryPlugin : public Plugin {
public:
    /**
     * Get the plugin type
     * 
     * @return Plugin type
     */
    virtual std::string getType() const { return "memory"; }

    /**
     * Apply plugin effects to memory systems
     * 
     * @param brain Brain containing memory systems
     * @param timestep Current simulation timestep
     */
    virtual void applyToMemory(class Brain* brain, SimulationStep timestep) {}

    /**
     * Get additional memory systems provided by the plugin
     * 
     * @return Vector of additional memory system names
     */
    virtual std::vector<std::string> getAdditionalMemorySystems() const { return {}; }

    /**
     * Get additional memory consolidation rules
     * 
     * @return Vector of additional consolidation rules
     */
    virtual std::vector<std::string> getAdditionalConsolidationRules() const { return {}; }
};

/**
 * Plasticity plugin interface
 * 
 * Extends plasticity functionality by adding new plasticity
 * rules, learning algorithms, or developmental effects.
 */
class PlasticityPlugin : public Plugin {
public:
    /**
     * Get the plugin type
     * 
     * @return Plugin type
     */
    virtual std::string getType() const { return "plasticity"; }

    /**
     * Apply plugin effects to plasticity systems
     * 
     * @param brain Brain containing plasticity systems
     * @param timestep Current simulation timestep
     * @param currentTime Current simulation time
     */
    virtual void applyToPlasticity(class Brain* brain, SimulationStep timestep, Timestamp currentTime) {}

    /**
     * Get additional plasticity rules provided by the plugin
     * 
     * @return Vector of additional plasticity rule names
     */
    virtual std::vector<std::string> getAdditionalPlasticityRules() const { return {}; }

    /**
     * Get additional developmental effects
     * 
     * @return Vector of additional developmental effect names
     */
    virtual std::vector<std::string> getAdditionalDevelopmentalEffects() const { return {}; }
};

/**
 * Neuromodulation plugin interface
 * 
 * Extends neuromodulation functionality by adding new
 * neuromodulators, learning algorithms, or interaction patterns.
 */
class NeuromodulationPlugin : public Plugin {
public:
    /**
     * Get the plugin type
     * 
     * @return Plugin type
     */
    virtual std::string getType() const { return "neuromodulation"; }

    /**
     * Apply plugin effects to neuromodulation systems
     * 
     * @param brain Brain containing neuromodulation systems
     * @param timestep Current simulation timestep
     */
    virtual void applyToNeuromodulation(class Brain* brain, SimulationStep timestep) {}

    /**
     * Get additional neuromodulators provided by the plugin
     * 
     * @return Vector of additional neuromodulator names
     */
    virtual std::vector<std::string> getAdditionalNeuromodulators() const { return {}; }

    /**
     * Get additional neuromodulation rules
     * 
     * @return Vector of additional neuromodulation rule names
     */
    virtual std::vector<std::string> getAdditionalNeuromodulationRules() const { return {}; }
};

/**
 * Plugin manager for loading, unloading, and managing plugins
 * 
 * The plugin manager handles plugin lifecycle, configuration,
 * and provides interfaces for plugin communication and coordination.
 */
class PluginManager {
public:
    PluginManager();
    ~PluginManager();

    /**
     * Load a plugin from a file
     * 
     * @param filepath Path to plugin file
     * @return Pointer to loaded plugin, or nullptr if loading failed
     */
    Plugin* loadPlugin(const std::string& filepath);

    /**
     * Load a plugin from a shared library
     * 
     * @param libraryPath Path to shared library
     * @param pluginName Name of plugin to load
     * @return Pointer to loaded plugin, or nullptr if loading failed
     */
    Plugin* loadPluginFromLibrary(const std::string& libraryPath, const std::string& pluginName);

    /**
     * Unload a plugin
     * 
     * @param plugin Pointer to plugin to unload
     */
    void unloadPlugin(Plugin* plugin);

    /**
     * Get a plugin by name
     * 
     * @param name Plugin name
     * @return Pointer to plugin, or nullptr if not found
     */
    Plugin* getPlugin(const std::string& name);

    /**
     * Get all loaded plugins
     * 
     * @return Vector of pointers to all loaded plugins
     */
    std::vector<Plugin*> getAllPlugins() const;

    /**
     * Get plugins of a specific type
     * 
     * @param type Plugin type
     * @return Vector of pointers to plugins of specified type
     */
    std::vector<Plugin*> getPluginsByType(const std::string& type) const;

    /**
     * Enable or disable a plugin
     * 
     * @param name Plugin name
     * @param enabled Whether to enable the plugin
     */
    void setPluginEnabled(const std::string& name, bool enabled);

    /**
     * Check if a plugin is enabled
     * 
     * @param name Plugin name
     * @return true if plugin is enabled
     */
    bool isPluginEnabled(const std::string& name) const;

    /**
     * Configure a plugin
     * 
     * @param name Plugin name
     * @param config Configuration for the plugin
     */
    void configurePlugin(const std::string& name, const std::any& config);

    /**
     * Apply all enabled plugins to a brain
     * 
     * @param brain Brain to modify
     * @param timestep Current simulation timestep
     * @param currentTime Current simulation time
     */
    void applyPluginsToBrain(class Brain* brain, SimulationStep timestep, Timestamp currentTime);

    /**
     * Apply all enabled plugins to memory systems
     * 
     * @param brain Brain containing memory systems
     * @param timestep Current simulation timestep
     */
    void applyPluginsToMemory(class Brain* brain, SimulationStep timestep);

    /**
     * Apply all enabled plugins to plasticity systems
     * 
     * @param brain Brain containing plasticity systems
     * @param timestep Current simulation timestep
     * @param currentTime Current simulation time
     */
    void applyPluginsToPlasticity(class Brain* brain, SimulationStep timestep, Timestamp currentTime);

    /**
     * Apply all enabled plugins to neuromodulation systems
     * 
     * @param brain Brain containing neuromodulation systems
     * @param timestep Current simulation timestep
     */
    void applyPluginsToNeuromodulation(class Brain* brain, SimulationStep timestep);

    /**
     * Save plugin state
     * 
     * @param filepath Filepath to save state
     */
    void savePluginState(const std::string& filepath);

    /**
     * Load plugin state
     * 
     * @param filepath Filepath to load state
     */
    void loadPluginState(const std::string& filepath);

    /**
     * Clear all loaded plugins
     */
    void clearPlugins();

private:
    struct PluginInfo {
        Plugin* plugin;
        std::string filepath;
        std::any config;
    };

    std::vector<PluginInfo> plugins;
    mutable std::mutex mutex;

    /**
     * Find plugin by name
     * 
     * @param name Plugin name
     * @return Iterator to plugin info, or end if not found
     */
    std::vector<PluginInfo>::const_iterator findPlugin(const std::string& name) const;
};

/**
 * Plugin factory for creating plugins
 * 
 * The plugin factory provides a way to create plugins without
 * loading them from files, which is useful for testing and
 * for creating plugins programmatically.
 */
class PluginFactory {
public:
    PluginFactory() = default;
    ~PluginFactory() = default;

    /**
     * Register a plugin class
     * 
     * @tparam PluginType Type of plugin to register
     * @param name Plugin name
     * @param creator Function that creates a plugin instance
     */
    template<typename PluginType, typename Creator>
    void registerPlugin(const std::string& name, Creator creator) {
        creators[name] = [creator](const std::any& config) -> Plugin* {
            // Create plugin instance
            PluginType* plugin = creator();
            // Initialize plugin
            plugin->initialize(config);
            return plugin;
        };
    }

    /**
     * Create a plugin
     * 
     * @param name Plugin name
     * @param config Plugin configuration
     * @return Pointer to created plugin
     */
    Plugin* createPlugin(const std::string& name, const std::any& config = std::any());

    /**
     * Create a plugin instance
     * 
     * @tparam PluginType Type of plugin
     * @tparam... Args Constructor arguments
     * @param args Constructor arguments
     * @return Pointer to created plugin
     */
    template<typename PluginType, typename... Args>
    Plugin* createPlugin(Args&&... args) {
        return new PluginType(std::forward<Args>(args)...);
    }

    /**
     * Check if a plugin is registered
     * 
     * @param name Plugin name
     * @return true if plugin is registered
     */
    bool isPluginRegistered(const std::string& name) const {
        return creators.find(name) != creators.end();
    }

    /**
     * Get all registered plugin names
     * 
     * @return Vector of registered plugin names
     */
    std::vector<std::string> getRegisteredPlugins() const {
        std::vector<std::string> names;
        for (const auto& pair : creators) {
            names.push_back(pair.first);
        }
        return names;
    }

private:
    std::unordered_map<std::string, std::function<Plugin*(const std::any&)>> creators;
};

/**
 * Plugin extension point for adding custom functionality
 * 
 * Extension points allow plugins to hook into specific parts of
 * the NLM system and modify their behavior.
 */
class ExtensionPoint {
public:
    virtual ~ExtensionPoint() = default;

    /**
     * Get the extension point name
     * 
     * @return Extension point name
     */
    virtual std::string getName() const = 0;

    /**
     * Get the extension point type
     * 
     * @return Extension point type
     */
    virtual std::string getType() const = 0;

    /**
     * Handle an event
     * 
     * @param event Event to handle
     * @param data Event data
     * @return true if event was handled, false otherwise
     */
    virtual bool handleEvent(const std::string& event, const std::any& data) = 0;

    /**
     * Get all handlers for an event
     * 
     * @param event Event name
     * @return Vector of event handlers
     */
    virtual std::vector<std::function<bool(const std::any&)>> getHandlers(const std::string& event) = 0;

    /**
     * Add an event handler
     * 
     * @param event Event name
     * @param handler Event handler
     */
    virtual void addHandler(const std::string& event, std::function<bool(const std::any&)> handler) = 0;

    /**
     * Remove an event handler
     * 
     * @param event Event name
     * @param handler Event handler
     */
    virtual void removeHandler(const std::::string& event, std::function<bool(const std::any&)> handler) = 0;

    /**
     * Get extension point configuration
     * 
     * @return Extension point configuration
     */
    virtual std::any getConfiguration() const = 0;

    /**
     * Set extension point configuration
     * 
     * @param config Extension point configuration
     */
    virtual void setConfiguration(const std::any& config) = 0;
};

/**
 * Plugin loader for loading plugins from different sources
 * 
 * The plugin loader provides interfaces for loading plugins
 * from files, shared libraries, and other sources.
 */
class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();

    /**
     * Load a plugin from a file
     * 
     * @param filepath Path to plugin file
     * @return Pointer to loaded plugin, or nullptr if loading failed
     */
    Plugin* loadFromFile(const std::string& filepath);

    /**
     * Load a plugin from a shared library
     * 
     * @param libraryPath Path to shared library
     * @param pluginName Name of plugin to load
     * @return Pointer to loaded plugin, or nullptr if loading failed
     */
    Plugin* loadFromLibrary(const std::string& libraryPath, const std::string& pluginName);

    /**
     * Load a plugin from a directory
     * 
     * @param directoryPath Path to plugin directory
     * @return Vector of pointers to loaded plugins
     */
    std::vector<Plugin*> loadFromDirectory(const std::string& directoryPath);

    /**
     * Load all plugins
     * 
     * @return Vector of pointers to all loaded plugins
     */
    std::vector<Plugin*> loadAllPlugins();

    /**
     * Unload a plugin
     * 
     * @param plugin Pointer to plugin to unload
     */
    void unloadPlugin(Plugin* plugin);

    /**
     * Get available plugins
     * 
     * @return Vector of available plugin names
     */
    std::vector<std::string> getAvailablePlugins() const;

    /**
     * Check if a plugin is available
     * 
     * @param name Plugin name
     * @return true if plugin is available
     */
    bool isPluginAvailable(const std::string& name) const;

private:
    std::unordered_map<std::string, std::function<Plugin*()>> pluginLoaders;
};

} // namespace nlm