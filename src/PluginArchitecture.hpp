#pragma once

/**
 * Plugin Architecture for NLM (Neural Learning Machine)
 * 
 * This module provides a plugin architecture for extending NLM functionality
 * without modifying the core codebase. Plugins can be loaded dynamically
 * to add new neural models, learning rules, visualization methods,
 * and experiment types.
 * 
 * This improves:
 * 1. Extensibility - Users can add custom functionality
 * 2. Testability - Plugins can be tested independently
 * 3. Modularity - Core code remains clean and focused
 * 4. Performance - Optional plugins don't impact performance
 * 5. Community - Shared plugins can be distributed
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <unordered_map>
#include <type_traits>

namespace nlm {

/**
 * Plugin lifecycle states
 */
enum class PluginState {
    NotLoaded,
    Loading,
    Loaded,
    Initializing,
    Initialized,
    Running,
    Stopping,
    Stopped,
    Unloading,
    Failed
};

/**
 * Plugin categories
 */
enum class PluginCategory {
    NeuralModel,
    LearningRule,
    Visualization,
    Environment,
    Experiment,
    DataExporter,
    Utility,
    Custom
};

/**
 * Plugin interface base class
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginCategory getCategory() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getContact() const = 0;
    
    // Lifecycle management
    virtual bool load() = 0;
    virtual bool unload() = 0;
    virtual bool initialize() = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    
    // Query state
    virtual PluginState getState() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    
    // Configuration
    virtual bool configure(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfiguration(const std::string& key) const = 0;
    
    // Query interface
    virtual bool queryInterface(const std::string& interface) const = 0;
    virtual bool supportsFeature(const std::string& feature) const = 0;
    
    // Extension points
    virtual std::vector<std::string> getExtensions() const = 0;
    virtual bool handleExtension(const std::string& extension, const std::string& data) = 0;
};

/**
 * Neural model plugin interface
 */
class INeuralModelPlugin : public virtual IPlugin {
public:
    virtual ~INeuralModelPlugin() = default;
    
    // Neuron creation
    virtual std::unique_ptr<class Neuron> createNeuron(NeuronId id) = 0;
    virtual std::unique_ptr<class Synapse> createSynapse(SynapseId id, NeuronId source, NeuronId destination) = 0;
    
    // Population management
    virtual std::unique_ptr<class NeuralPopulation> createPopulation(PopulationId id, size_t size) = 0;
    
    // Parameter configuration
    virtual bool configureParameters(const std::string& context, const std::string& key, double value) = 0;
    virtual std::string getParameter(const std::string& context, const std::string& key) const = 0;
    
    // Model-specific operations
    virtual bool validateModel() const = 0;
    virtual std::string getModelType() const = 0;
    virtual double getDefaultThreshold() const = 0;
    virtual double getDefaultRestingPotential() const = 0;
};

/**
 * Learning rule plugin interface
 */
class ILearningRulePlugin : public virtual IPlugin {
public:
    virtual ~ILearningRulePlugin() = default;
    
    // Plasticity management
    virtual bool enablePlasticity(Synapse* synapse, bool hebbian, bool stdp, bool rewardModulated) = 0;
    virtual float computeWeightChange(Synapse* synapse, const std::vector<Timestamp>& preSpikeHistory, 
                                    const std::vector<Timestamp>& postSpikeHistory, float currentWeight) = 0;
    virtual bool validatePlasticityParameters(float ltpWeight, float ltdWeight, float tau) const = 0;
    
    // Learning algorithm management
    virtual bool setLearningRate(float rate) = 0;
    virtual bool setTimeConstant(float tau) = 0;
    virtual bool setLTPWeight(float weight) = 0;
    virtual bool setLTDWeight(float weight) = 0;
    
    // Learning algorithm identification
    virtual std::string getAlgorithmName() const = 0;
    virtual std::string getAlgorithmDescription() const = 0;
    virtual bool supportsSTDP() const = 0;
    virtual bool supportsHebbian() const = 0;
    virtual bool supportsRewardModulated() const = 0;
};

/**
 * Visualization plugin interface
 */
class IVisualizationPlugin : public virtual IPlugin {
public:
    virtual ~IVisualizationPlugin() = default;
    
    // Visualization operations
    virtual bool render(const class Brain* brain, const std::string& outputFormat) = 0;
    virtual bool createAnimation(const class Brain* brain, const std::string& filename, 
                                size_t frames, double duration) = 0;
    virtual bool createSnapshot(const class Brain* brain, const std::string& filename) = 0;
    
    // Visualization configuration
    virtual bool setColorMap(const std::string& colormap) = 0;
    virtual bool setScale(double scale) = 0;
    virtual bool setStyle(const std::string& style) = 0;
    
    // Visualization parameters
    virtual std::vector<std::string> getAvailableColorMaps() const = 0;
    virtual std::vector<std::string> getAvailableStyles() const = 0;
    virtual std::string getDefaultFormat() const = 0;
};

/**
 * Environment plugin interface
 */
class IEnvironmentPlugin : public virtual IPlugin {
public:
    virtual ~IEnvironmentPlugin() = default;
    
    // Environment simulation
    virtual bool initialize(double width, double height, double depth) = 0;
    virtual bool step(double timeStep) = 0;
    virtual void reset() = 0;
    
    // Environmental stimuli
    virtual std::vector<float> getSensoryInput(const class Neuron* neuron) const = 0;
    virtual std::vector<float> getMotorOutput(const class Neuron* neuron) const = 0;
    
    // Environmental properties
    virtual double getTemperature() const = 0;
    virtual double getHumidity() const = 0;
    virtual double getLightIntensity() const = 0;
    virtual std::string getEnvironmentType() const = 0;
};

/**
 * Data exporter plugin interface
 */
class IDataExporterPlugin : public virtual IPlugin {
public:
    virtual ~IDataExporterPlugin() = default;
    
    // Data export operations
    virtual bool exportBrainState(const class Brain* brain, const std::string& filename) = 0;
    virtual bool exportExperimentData(const std::vector<std::shared_ptr<class Experiment>>& experiments, 
                                    const std::string& filename) = 0;
    virtual bool exportMetrics(const std::string& metricsName, const std::string& filename) = 0;
    
    // Export formats
    virtual std::vector<std::string> getSupportedFormats() const = 0;
    virtual bool setExportFormat(const std::string& format) = 0;
    virtual std::string getExportFormat() const = 0;
    
    // Export configuration
    virtual bool setExportOptions(const std::string& key, const std::string& value) = 0;
    virtual std::string getExportOption(const std::string& key) const = 0;
};

/**
 * Plugin loader interface
 */
class IPluginLoader {
public:
    virtual ~IPluginLoader() = default;
    
    // Plugin loading
    virtual std::shared_ptr<IPlugin> loadPlugin(const std::string& pluginPath) = 0;
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    virtual bool reloadPlugin(const std::string& pluginName) = 0;
    
    // Plugin discovery
    virtual std::vector<std::string> discoverPlugins(const std::string& directory) = 0;
    virtual std::shared_ptr<IPlugin> findPlugin(const std::string& name) = 0;
    
    // Plugin management
    virtual bool enablePlugin(const std::string& name) = 0;
    virtual bool disablePlugin(const std::string& name) = 0;
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
    virtual std::map<std::string, PluginState> getPluginStates() const = 0;
    
    // Plugin configuration
    virtual bool configurePlugin(const std::string& name, const std::string& key, const std::string& value) = 0;
    virtual std::string getPluginConfig(const std::string& name, const std::string& key) const = 0;
    
    // Query interface
    virtual bool hasInterface(const std::string& interface) const = 0;
    virtual std::vector<std::string> getAvailableInterfaces() const = 0;
    virtual std::shared_ptr<IPlugin> createPlugin(const std::string& type, const std::string& name) = 0;
};

/**
 * Plugin manager
 */
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Singleton access
    static PluginManager& getInstance();
    
    // Plugin loading
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool reloadPlugin(const std::string& pluginName);
    
    // Plugin discovery and management
    std::vector<std::string> discoverPlugins(const std::string& directory);
    std::vector<std::string> getLoadedPlugins() const;
    std::map<std::string, PluginState> getPluginStates() const;
    
    // Plugin access
    std::shared_ptr<IPlugin> getPlugin(const std::string& name);
    std::vector<std::shared_ptr<IPlugin>> getPluginsByCategory(PluginCategory category);
    std::vector<std::shared_ptr<IPlugin>> getPluginsByInterface(const std::string& interface);
    
    // Plugin configuration
    bool configurePlugin(const std::string& name, const std::string& key, const std::string& value);
    std::string getPluginConfig(const std::string& name, const std::string& key) const;
    
    // Plugin lifecycle
    bool startAllPlugins();
    bool stopAllPlugins();
    bool initializeAllPlugins();
    bool unloadAllPlugins();
    
    // System integration
    bool integratePluginWithBrain(std::shared_ptr<IPlugin> plugin, std::shared_ptr<class Brain> brain);
    bool integratePluginWithEnvironment(std::shared_ptr<IPlugin> plugin, std::shared_ptr<class Environment> environment);
    
    // Plugin information
    std::string getPluginInfo(const std::string& name) const;
    std::string getSystemInfo() const;
    std::string getPluginDependencyInfo(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
};

/**
 * Plugin registry
 */
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Plugin registration
    bool registerPlugin(const std::string& name, std::function<std::shared_ptr<IPlugin>()> factory);
    bool unregisterPlugin(const std::string& name);
    bool unregisterAllPlugins();
    
    // Plugin discovery
    std::vector<std::string> getRegisteredPlugins() const;
    std::shared_ptr<IPlugin> createPlugin(const std::string& name);
    std::shared_ptr<IPlugin> createPluginByType(const std::string& type, const std::string& name);
    
    // Plugin information
    std::string getPluginDescription(const std::string& name) const;
    PluginCategory getPluginCategory(const std::string& name) const;
    std::vector<std::string> getPluginDependencies(const std::string& name) const;
    
    // Plugin filtering
    std::vector<std::string> findPluginsByCategory(PluginCategory category) const;
    std::vector<std::string> findPluginsByInterface(const std::string& interface) const;
    std::vector<std::string> findPluginsByKeyword(const std::string& keyword) const;
    
    // Plugin configuration
    bool setPluginDefaultConfig(const std::string& name, const std::string& key, const std::string& value);
    std::string getPluginDefaultConfig(const std::string& name, const std::string& key) const;
    
    // Plugin metadata
    std::string getPluginAuthor(const std::string& name) const;
    std::string getPluginVersion(const std::string& name) const;
    std::string getPluginContact(const std::string& name) const;
    std::string getPluginLicense(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
};

/**
 * Plugin utilities
 */
class PluginUtilities {
public:
    // Plugin validation
    static bool validatePluginName(const std::string& name);
    static bool validatePluginVersion(const std::string& version);
    static bool validatePluginInterface(const std::string& interface);
    static bool validatePluginCategory(PluginCategory category);
    
    // Plugin discovery
    static std::vector<std::string> discoverPlugins(const std::string& directory, 
                                                   const std::vector<std::string>& extensions = {});
    static std::string getPluginInfoString(const std::shared_ptr<IPlugin>& plugin);
    static std::string getSystemInfoString();
    
    // Plugin configuration
    static std::string formatPluginConfig(const std::map<std::string, std::string>& config);
    static bool parsePluginConfig(const std::string& configString, 
                                 std::map<std::string, std::string>& config);
    
    // Plugin dependency management
    static bool checkPluginDependencies(const std::vector<std::string>& dependencies,
                                       const std::map<std::string, PluginState>& pluginStates);
    static std::string getDependencyError(const std::vector<std::string>& dependencies,
                                         const std::map<std::string, PluginState>& pluginStates);
    
    // Plugin lifecycle
    static std::string getStateString(PluginState state);
    static PluginState getStateFromString(const std::string& stateStr);
    
    // Plugin category
    static std::string getCategoryString(PluginCategory category);
    static PluginCategory getCategoryFromString(const std::string& categoryStr);
    
private:
    PluginUtilities() = delete;
    PluginUtilities(const PluginUtilities&) = delete;
    PluginUtilities& operator=(const PluginUtilities&) = delete;
};

} // namespace nlm
