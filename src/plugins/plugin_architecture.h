// Plugin Architecture for NLM
// Plugin System Implementation
//
// This document describes the plugin architecture for the NLM (Neural Learning Machine)
// framework, enabling extensibility and custom functionality development.
//
// The plugin system allows users to:
// 1. Create custom neural network components
// 2. Extend existing systems with custom functionality
// 3. Integrate third-party algorithms and models
// 4. Create custom visualization and analysis tools
// 5. Develop specialized configuration systems
//
// The architecture follows these principles:
// - Loose coupling between core systems and plugins
// - Well-defined extension points
// - Plugin lifecycle management
// - Security and stability guarantees
// - Performance optimization considerations

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <any>

namespace nlm {

// Plugin System Types
enum class PluginType {
    NEURAL_COMPONENT,
    MEMORY_SYSTEM,
    PLASTICITY_RULE,
    NEUROMODULATION,
    SENSORY_PROCESSOR,
    MOTOR_CONTROLLER,
    VISUALIZATION,
    ANALYSIS,
    UTILITY,
    CUSTOM
};

enum class PluginState {
    UNLOADED,
    LOADING,
    LOADED,
    RUNNING,
    STOPPING,
    ERROR,
    UNLOADING
};

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Basic plugin information
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginType getType() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getContact() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::map<std::string, std::any>& config) = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool unload() = 0;
    
    // State management
    virtual PluginState getState() const = 0;
    virtual std::map<std::string, std::any> getStatus() const = 0;
    
    // Event handling
    virtual void onEvent(const std::string& eventName, const std::map<std::string, std::any>& data) = 0;
    
    // Configuration management
    virtual bool configure(const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getConfiguration() const = 0;
    
    // Plugin discovery
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual std::vector<std::string> getProvides() const = 0;
};

// Extension point interface
class IExtensionPoint {
public:
    virtual ~IExtensionPoint() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginType getPluginType() const = 0;
    
    virtual bool registerPlugin(std::shared_ptr<IPlugin> plugin) = 0;
    virtual bool unregisterPlugin(const std::string& pluginName) = 0;
    
    virtual std::vector<std::shared_ptr<IPlugin>> getPlugins() const = 0;
    virtual std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName) const = 0;
    
    virtual std::map<std::string, std::any> getExtensionData() const = 0;
};

// Neural component plugin interface
class INeuralComponent : public virtual IPlugin {
public:
    virtual ~INeuralComponent() = default;
    
    // Neuron factory methods
    virtual std::string createNeuron(const std::map<std::string, std::any>& params) = 0;
    virtual bool initializeNeuron(const std::string& neuronId, const std::map<std::string, std::any>& params) = 0;
    virtual bool updateNeuron(const std::string& neuronId, double timestep) = 0;
    virtual bool destroyNeuron(const std::string& neuronId) = 0;
    
    // Synapse management
    virtual std::string createSynapse(const std::string& preNeuron, const std::string& postNeuron, 
                                    const std::map<std::string, std::any>& params) = 0;
    virtual bool updateSynapse(const std::string& synapseId, double timestep) = 0;
    virtual bool removeSynapse(const std::string& synapseId) = 0;
    
    // Network operations
    virtual bool connectComponents(const std::map<std::string, std::any>& connections) = 0;
    virtual bool simulateStep(double timestep) = 0;
};

// Memory system plugin interface
class IMemorySystem : public virtual IPlugin {
public:
    virtual ~IMemorySystem() = default;
    
    // Working memory operations
    virtual bool store(const std::string& key, const std::map<std::string, std::any>& data) = 0;
    virtual std::map<std::string, std::any> retrieve(const std::string& key) = 0;
    virtual bool contains(const std::string& key) const = 0;
    virtual bool remove(const std::string& key) = 0;
    virtual void clear() = 0;
    virtual std::vector<std::string> getKeys() const = 0;
    
    // Episodic memory operations
    virtual std::string storeEpisode(const std::map<std::string, std::any>& episode) = 0;
    virtual std::map<std::string, std::any> retrieveEpisode(const std::string& episodeId) = 0;
    virtual std::vector<std::string> getEpisodes() const = 0;
    virtual bool deleteEpisode(const std::string& episodeId) = 0;
    
    // Memory consolidation
    virtual void consolidate() = 0;
    virtual void replay(std::function<void(const std::map<std::string, std::any>&)> callback) = 0;
};

// Plasticity rule plugin interface
class IPlasticityRule : public virtual IPlugin {
public:
    virtual ~IPlasticityRule() = default;
    
    // Plasticity computation
    virtual float computeWeightChange(const std::map<std::string, std::any>& preSpike,
                                    const std::map<std::string, std::any>& postSpike,
                                    const std::map<std::string, std::any>& synapseState) = 0;
    
    // Rule configuration
    virtual bool configureRule(const std::map<std::string, std::any>& parameters) = 0;
    virtual std::map<std::string, std::any> getRuleParameters() const = 0;
    
    // Learning control
    virtual void enableLearning(bool enable) = 0;
    virtual bool isLearningEnabled() const = 0;
    virtual void setLearningRate(float rate) = 0;
    virtual float getLearningRate() const = 0;
};

// Neuromodulation plugin interface
class INeuromodulation : public virtual IPlugin {
public:
    virtual ~INeuromodulation() = default;
    
    // Neuromodulator state
    virtual void setLevel(float level) = 0;
    virtual float getLevel() const = 0;
    virtual void setSensitivity(float sensitivity) = 0;
    virtual float getSensitivity() const = 0;
    
    // Effect application
    virtual void applyToNeuron(const std::string& neuronId, float modulation) = 0;
    virtual void applyToSynapse(const std::string& synapseId, float modulation) = 0;
    virtual void applyToNetwork() = 0;
    
    // Neuromodulator interactions
    virtual void setDependency(const std::string& otherModulator, float strength) = 0;
    virtual float getDependency(const std::string& otherModulator) const = 0;
};

// Sensory processor plugin interface
class ISensoryProcessor : public virtual IPlugin {
public:
    virtual ~ISensoryProcessor() = default;
    
    // Sensory input processing
    virtual std::vector<float> processInput(const std::vector<float>& rawInput) = 0;
    virtual std::vector<float> filterInput(const std::vector<float>& input, const std::string& filterType) = 0;
    virtual void addNoise(float noiseLevel) = 0;
    
    // Sensory modalities
    virtual bool supportsVision() const = 0;
    virtual bool supportsAudio() const = 0;
    virtual bool supportsTouch() const = 0;
    virtual bool supportsProprioception() const = 0;
    
    // Sensory configuration
    virtual void setTemporalFilter(float filter) = 0;
    virtual void setSpatialFilter(float filter) = 0;
    virtual void setNoiseLevel(float noise) = 0;
};

// Motor controller plugin interface
class IMotorController : public virtual IPlugin {
public:
    virtual ~IMotorController() = default;
    
    // Action production
    virtual std::map<std::string, std::any> produceAction(const std::map<std::string, std::any>& neuralActivity) = 0;
    virtual bool executeAction(const std::map<std::string, std::any>& action) = 0;
    virtual void stopAction(const std::map<std::string, std::any>& action) = 0;
    
    // Motor configuration
    virtual void setMaxSpeed(float speed) = 0;
    virtual void setAcceleration(float accel) = 0;
    virtual void setResponsiveness(float resp) = 0;
    
    // Action control
    virtual bool isActionAvailable(const std::string& actionType) const = 0;
    virtual std::vector<std::string> getAvailableActions() const = 0;
};

// Visualization plugin interface
class IVisualization : public virtual IPlugin {
public:
    virtual ~IVisualization() = default;
    
    // Visualization methods
    virtual void displayNeurons(const std::vector<std::string>& neuronIds) = 0;
    virtual void displaySynapses(const std::vector<std::string>& synapseIds) = 0;
    virtual void displayNetwork(std::function<void(const std::map<std::string, std::any>&)> drawCallback) = 0;
    virtual void displayMetrics(const std::map<std::string, std::any>& metrics) = 0;
    
    // Visualization configuration
    virtual void setScale(float scale) = 0;
    virtual void setColorScheme(const std::string& scheme) = 0;
    virtual void setRefreshRate(float rate) = 0;
    
    // Interaction
    virtual bool handleClick(const std::map<std::string, std::any>& clickData) = 0;
    virtual bool handleZoom(float zoomLevel) = 0;
    virtual bool handlePan(const std::map<std::string, std::any>& panData) = 0;
};

// Analysis plugin interface
class IAnalysis : public virtual IPlugin {
public:
    virtual ~IAnalysis() = default;
    
    // Analysis methods
    virtual std::map<std::string, std::any> analyzeNeurons(const std::vector<std::string>& neuronIds) = 0;
    virtual std::map<std::string, std::any> analyzeSynapses(const std::vector<std::string>& synapseIds) = 0;
    virtual std::map<std::string, std::any> analyzeNetwork() = 0;
    virtual std::map<std::string, std::any> detectPatterns(const std::map<std::string, std::any>& data) = 0;
    
    // Analysis configuration
    virtual void setAnalysisWindow(int windowSize) = 0;
    virtual void setDetectionThreshold(float threshold) = 0;
    virtual void enableOutlierDetection(bool enable) = 0;
    
    // Result processing
    virtual void setResultCallback(std::function<void(const std::map<std::string, std::any>&)> callback) = 0;
    virtual std::vector<std::map<std::string, std::any>> getResults() const = 0;
    virtual void clearResults() = 0;
};

// Plugin manager
class PluginManager {
private:
    std::map<std::string, std::shared_ptr<IPlugin>> plugins;
    std::map<PluginType, std::vector<std::string>> pluginsByType;
    std::map<std::string, std::shared_ptr<IExtensionPoint>> extensionPoints;
    
    // Plugin loading
    std::map<std::string, std::function<std::shared_ptr<IPlugin>()>> pluginFactories;
    std::map<std::string, std::string> pluginFileExtensions;
    
    // State management
    std::map<std::string, PluginState> pluginStates;
    std::chrono::system_clock::time_point lastSaveTime;
    
    // Event system
    std::map<std::string, std::vector<std::function<void(const std::string&, const std::map<std::string, std::any>&)>>
        eventHandlers;
    
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin management
    bool loadPlugin(const std::string& pluginPath);
    bool loadPlugin(std::shared_ptr<IPlugin> plugin);
    bool unloadPlugin(const std::string& pluginName);
    bool reloadPlugin(const std::string& pluginName);
    
    bool registerPlugin(std::shared_ptr<IPlugin> plugin);
    bool unregisterPlugin(const std::string& pluginName);
    bool updatePlugin(const std::string& pluginName, std::shared_ptr<IPlugin> newPlugin);
    
    // Plugin discovery
    std::vector<std::string> getPluginNames() const;
    std::vector<std::shared_ptr<IPlugin>> getPlugins(PluginType type) const;
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName) const;
    bool hasPlugin(const std::string& pluginName) const;
    
    // Extension points
    bool registerExtensionPoint(std::shared_ptr<IExtensionPoint> extensionPoint);
    bool unregisterExtensionPoint(const std::string& extensionPointName);
    std::shared_ptr<IExtensionPoint> getExtensionPoint(const std::string& extensionPointName) const;
    
    // Plugin configuration
    bool configurePlugin(const std::string& pluginName, const std::map<std::string, std::any>& config);
    std::map<std::string, std::any> getPluginConfig(const std::string& pluginName) const;
    
    // Plugin lifecycle
    bool startPlugin(const std::string& pluginName);
    bool stopPlugin(const std::string& pluginName);
    bool pausePlugin(const std::string& pluginName);
    bool resumePlugin(const std::string& pluginName);
    
    // Plugin information
    std::map<std::string, std::any> getPluginInfo(const std::string& pluginName) const;
    std::vector<std::map<std::string, std::any>> getAllPluginInfo() const;
    
    // Plugin dependencies
    bool checkPluginDependencies(const std::string& pluginName) const;
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;
    bool satisfyDependencies(const std::string& pluginName);
    
    // Plugin search
    std::vector<std::string> findPlugins(PluginType type, const std::map<std::string, std::any>& criteria = {}) const;
    std::vector<std::string> findPluginsByName(const std::string& namePattern) const;
    
    // Plugin services
    template<typename T>
    std::shared_ptr<T> getPluginService(const std::string& pluginName) const;
    
    // Events
    void registerEventHandler(const std::string& eventName,
                           std::function<void(const std::string&, const std::map<std::string, std::any>&)> handler);
    void triggerEvent(const std::string& eventName, const std::map<std::string, std::any>& data);
    
    // Save and load
    bool savePluginState(const std::string& filename);
    bool loadPluginState(const std::string& filename);
    bool exportPluginConfiguration(const std::string& pluginName, const std::string& filename);
    bool importPluginConfiguration(const std::string& pluginName, const std::string& filename);
    
    // System management
    void shutdown();
    void cleanup();
    bool isHealthy() const;
    std::map<std::string, std::any> getSystemStatus() const;
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins() const;
    bool isPluginAvailable(const std::string& pluginPath) const;
    std::string getPluginVersion(const std::string& pluginName) const;
    
    // Advanced features
    bool enablePluginAutoLoad(const std::string& pluginPath, bool enable = true);
    bool enablePluginAutoStart(const std::string& pluginName, bool enable = true);
    bool enablePluginHotSwap(const std::string& pluginName, bool enable = true);
    
    // Monitoring
    std::map<std::string, std::any> getPluginMetrics(const std::string& pluginName) const;
    std::map<std::string, std::any> getSystemMetrics() const;
    void resetMetrics();
    
    // Plugin communication
    bool sendPluginMessage(const std::string& fromPlugin, const std::string& toPlugin,
                         const std::string& message, const std::map<std::string, std::any>& data = {});
    std::vector<std::map<std::string, std::any>> receivePluginMessages(const std::string& pluginName);
    void clearPluginMessages(const std::string& pluginName);
};

// Plugin factory
class PluginFactory {
public:
    static std::shared_ptr<IPlugin> createPlugin(const std::string& pluginType, 
                                                const std::map<std::string, std::any>& config = {});
    
    static bool registerPluginType(PluginType type,
                                  std::function<std::shared_ptr<IPlugin>(const std::map<std::string, std::any>&)> factory);
    
    static std::vector<PluginType> getAvailablePluginTypes();
    static std::string getPluginTypeName(PluginType type);
    static std::map<std::string, std::any> getPluginTypeInfo(PluginType type);
};

// Plugin utilities
namespace PluginUtils {
    std::string generatePluginId();
    bool validatePluginName(const std::string& name);
    bool validatePluginVersion(const std::string& version);
    std::string getPluginSignature(const std::shared_ptr<IPlugin>& plugin);
    bool verifyPluginSignature(const std::shared_ptr<IPlugin>& plugin, const std::string& signature);
    
    std::map<std::string, std::any> createPluginConfig(const std::string& key, const std::any& value);
    bool mergePluginConfigs(std::map<std::string, std::any>& target,
                          const std::map<std::string, std::any>& source);
    
    std::vector<std::string> getPluginDependencies(const std::map<std::string, std::any>& config);
    bool checkDependencySatisfaction(const std::string& pluginName,
                                   const std::map<std::string, std::any>& dependencies);
    
    std::string formatPluginError(const std::string& operation, const std::string& pluginName,
                                const std::exception& e);
    void logPluginEvent(const std::string& pluginName, const std::string& event,
                      const std::map<std::string, std::any>& data = {});
}

// Global plugin manager instance
global PluginManager& getPluginManager();

// Plugin macros
#define REGISTER_PLUGIN(PluginClass) \
    extern "C" std::shared_ptr<IPlugin> createPluginInstance() { \
        return std::make_shared<PluginClass>(); \
    }

#define PLUGIN_VERSION(major, minor, patch) \
    static const std::string PLUGIN_VERSION = #major "." #minor "." #patch

#define PLUGIN_INFO(name, version, description, author, contact) \
    const std::string PLUGIN_NAME = name; \
    const std::string PLUGIN_VERSION = version; \
    const std::string PLUGIN_DESCRIPTION = description; \
    const std::string PLUGIN_AUTHOR = author; \
    const std::string PLUGIN_CONTACT = contact

// Plugin lifecycle macros
#define PLUGIN_INITIALIZE() \
    virtual bool initialize(const std::map<std::string, std::any>& config) override

#define PLUGIN_START() \
    virtual bool start() override

#define PLUGIN_STOP() \
    virtual bool stop() override

#define PLUGIN_UNLOAD() \
    virtual bool unload() override

#define PLUGIN_CONFIGURE() \
    virtual bool configure(const std::map<std::string, std::any>& config) override

} // namespace nlm

// Plugin loading utility functions
namespace PluginLoader {
    bool loadPluginFromFile(const std::string& filename);
    bool loadPluginFromDirectory(const std::string& directory);
    bool unloadAllPlugins();
    std::vector<std::string> getAvailablePlugins() const;
    bool validatePluginFile(const std::string& filename);
    std::string getPluginFileType(const std::string& filename);
}

// Plugin security utilities
namespace PluginSecurity {
    bool validatePluginSecurity(const std::shared_ptr<nlm::IPlugin>& plugin);
    bool checkPluginPermissions(const std::string& pluginName, const std::string& permission);
    bool grantPluginPermission(const std::string& pluginName, const std::string& permission);
    bool revokePluginPermission(const std::string& pluginName, const std::string& permission);
    std::vector<std::string> getPluginPermissions(const std::string& pluginName);
}