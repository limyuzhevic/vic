#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <typeindex>
#include "core/Config/Config.hpp"

namespace nlm {

// Plugin interface base class
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Get plugin name
    virtual std::string getName() const = 0;
    
    // Get plugin version
    virtual std::string getVersion() const = 0;
    
    // Get plugin description
    virtual std::string getDescription() const = 0;
    
    // Initialize plugin
    virtual bool initialize(std::shared_ptr<Config> config) = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Check if plugin is enabled
    virtual bool isEnabled() const = 0;
    
    // Set plugin enabled state
    virtual void setEnabled(bool enabled) = 0;
};

// Neuron model plugin interface
class NeuronModelPlugin : public PluginInterface {
public:
    virtual std::unique_ptr<class Neuron> createNeuron(class NeuronId id) = 0;
    virtual NeuronType getSupportedNeuronType() const = 0;
    virtual bool supportsDynamics(const std::string& dynamicsType) const = 0;
};

// Synapse model plugin interface
class SynapseModelPlugin : public PluginInterface {
public:
    virtual std::unique_ptr<class Synapse> createSynapse(class SynapseId id, 
                                                        class NeuronId source,
                                                        class NeuronId destination) = 0;
    virtual bool supportsPlasticity(class PlasticityType type) const = 0;
    virtual double getDefaultWeight() const = 0;
};

// Plasticity rule plugin interface
class PlasticityRulePlugin : public PluginInterface {
public:
    virtual void applyPlasticity(class Synapse* synapse,
                               class Neuron* preNeuron,
                               class Neuron* postNeuron,
                               double weight,
                               double dt) = 0;
    virtual std::string getPlasticityType() const = 0;
    virtual double getLearningRate() const = 0;
};

// Neuromodulator plugin interface
class NeuromodulatorPlugin : public PluginInterface {
public:
    virtual void update(class AgentBrain* agentBrain, double dt) = 0;
    virtual class NeuromodulatorType getType() const = 0;
    virtual double getLevel() const = 0;
};

// Memory system plugin interface
class MemoryPlugin : public PluginInterface {
public:
    virtual void encode(class Experience experience) = 0;
    virtual std::vector<class Experience> retrieve(class Pattern pattern, size_t limit) = 0;
    virtual void consolidate() = 0;
    virtual void reset() = 0;
    virtual double getEncodingRate() const = 0;
    virtual double getRetrievalAccuracy() const = 0;
};

// Sensor plugin interface
class SensorPlugin : public PluginInterface {
public:
    virtual std::string getSensorType() const = 0;
    virtual std::vector<double> read(class SimpleWorld* world) = 0;
    virtual size_t getFeatureCount() const = 0;
    virtual std::vector<std::string> getFeatureNames() const = 0;
};

// Actuator plugin interface
class ActuatorPlugin : public PluginInterface {
public:
    virtual std::string getActuatorType() const = 0;
    virtual void apply(class MotorCommand command, class SimpleWorld* world, double dt) = 0;
    virtual bool isCommandSupported(class MotorCommand command) const = 0;
};

// Event handler plugin interface
class EventHandlerPlugin : public PluginInterface {
public:
    virtual bool handleEvent(class Event event, std::shared_ptr<Config> config) = 0;
    virtual std::vector<class EventType> getHandledEvents() const = 0;
    virtual int getPriority() const = 0;
};

// Visualization plugin interface
class VisualizationPlugin : public PluginInterface {
public:
    virtual void update(class VisualizationInterface* visualizer, 
                       class SimulationStep step,
                       class Timestamp time) = 0;
    virtual std::string getVisualizationType() const = 0;
    virtual bool requiresRealTime() const = 0;
};

// Main plugin manager
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Register a plugin
    bool registerPlugin(std::shared_ptr<PluginInterface> plugin);
    
    // Unregister a plugin
    bool unregisterPlugin(const std::string& name);
    
    // Get plugin by name
    std::shared_ptr<PluginInterface> getPlugin(const std::string& name);
    
    // Get plugin by type
    template<typename T>
    std::shared_ptr<T> getPlugin();
    
    // Get all plugins of a specific type
    template<typename T>
    std::vector<std::shared_ptr<T>> getPluginsOfType();
    
    // Initialize all plugins
    bool initializePlugins(std::shared_ptr<Config> config);
    
    // Shutdown all plugins
    void shutdownPlugins();
    
    // Enable/disable plugin
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    
    // Check if plugin is enabled
    bool isPluginEnabled(const std::string& name) const;
    
    // Load plugins from directory
    bool loadPluginsFromDirectory(const std::string& directory);
    
    // Save plugin configuration
    bool savePluginConfig(const std::string& filename) const;
    
    // Load plugin configuration
    bool loadPluginConfig(const std::string& filename);
    
    // Get list of all plugins
    std::vector<std::string> getPluginNames() const;
    
    // Get plugin info
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string type;
        bool enabled;
        std::string filePath;
    };
    
    std::vector<PluginInfo> getPluginInfo() const;
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory) const;
    
    // Plugin validation
    bool validatePlugin(std::shared_ptr<PluginInterface> plugin) const;
    
    // Plugin dependency management
    bool addPluginDependency(const std::string& pluginName, const std::string& dependencyName);
    bool removePluginDependency(const std::string& pluginName, const std::string& dependencyName);
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;
    
    // Plugin configuration
    void setPluginConfig(const std::string& pluginName, const std::string& key, const ConfigValue& value);
    std::optional<ConfigValue> getPluginConfig(const std::string& pluginName, const std::string& key) const;
    
    // Plugin statistics
    struct PluginStats {
        std::string name;
        uint64_t loadTime;
        uint64_t lastUsedTime;
        uint64_t callCount;
        uint64_t errorCount;
        double memoryUsage;
    };
    
    std::vector<PluginStats> getPluginStats() const;
    
    // Plugin lifecycle hooks
    virtual void onPluginLoaded(std::shared_ptr<PluginInterface> plugin) {}
    virtual void onPluginUnloaded(std::shared_ptr<PluginInterface> plugin) {}
    virtual void onPluginEnabled(std::shared_ptr<PluginInterface> plugin) {}
    virtual void onPluginDisabled(std::shared_ptr<PluginInterface> plugin) {}
    
protected:
    // Plugin registry
    std::map<std::string, std::shared_ptr<PluginInterface>> plugins_;
    
    // Plugin dependencies
    std::map<std::string, std::vector<std::string>> dependencies_;
    
    // Plugin configuration
    std::map<std::string, std::map<std::string, ConfigValue>> pluginConfigs_;
    
    // Plugin statistics
    std::map<std::string, PluginStats> pluginStats_;
    
    // Plugin discovery cache
    std::vector<std::string> pluginDiscoveryCache_;
    
    // Plugin loading cache
    std::map<std::type_index, std::vector<std::shared_ptr<PluginInterface>>> typeCache_;
};

// Plugin factory for creating instances
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    
    // Create plugin instance from configuration
    virtual std::shared_ptr<PluginInterface> createPlugin(const std::string& name,
                                                         std::shared_ptr<Config> config) = 0;
    
    // Get supported plugin types
    virtual std::vector<std::string> getSupportedPluginTypes() const = 0;
    
    // Validate plugin configuration
    virtual bool validatePluginConfig(const std::string& pluginName,
                                     const std::map<std::string, ConfigValue>& config) const = 0;
};

// Plugin loader for dynamic loading
class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();
    
    // Load plugin from shared library
    std::shared_ptr<PluginInterface> loadPlugin(const std::string& libraryPath,
                                               const std::string& pluginName);
    
    // Unload plugin
    bool unloadPlugin(const std::string& pluginName);
    
    // Get loaded plugin names
    std::vector<std::string> getLoadedPluginNames() const;
    
    // Set plugin library search paths
    void setLibraryPaths(const std::vector<std::string>& paths) {
        libraryPaths_ = paths;
    }
    
    // Get plugin library search paths
    std::vector<std::string> getLibraryPaths() const {
        return libraryPaths_;
    }
    
protected:
    // Plugin library cache
    std::map<std::string, void*> pluginLibraries_;
    
    // Plugin library search paths
    std::vector<std::string> libraryPaths_;
    
    // Plugin factory function
    using PluginFactoryFunc = std::function<std::shared_ptr<PluginInterface>(std::shared_ptr<Config>)>;
    
    // Plugin symbol name
    static const std::string getPluginSymbolName(const std::string& pluginName);
};

} // namespace nlm