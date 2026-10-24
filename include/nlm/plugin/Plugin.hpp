#ifndef NLM_PLUGIN_HPP
#define NLM_PLUGIN_HPP

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <any>

namespace nlm {
namespace plugin {

// Plugin system version
#define NLM_PLUGIN_VERSION "1.0.0"

// Plugin types
enum class PluginType {
    BrainComponent,
    PlasticityRule,
    Environment,
    MemorySystem,
    CognitionSystem,
    Neuromodulation,
    Experiment,
    Utility,
    Visualization,
    Statistics
};

// Plugin interface
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginType getType() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getWebsite() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::map<std::string, std::any>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Plugin discovery
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual std::vector<std::string> getOptionalDependencies() const = 0;
    
    // Plugin functionality
    virtual bool canHandle(const std::string& key) const = 0;
    virtual std::any execute(const std::string& operation, const std::map<std::string, std::any>& parameters) = 0;
};

// Plugin factory interface
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    
    virtual std::string getFactoryName() const = 0;
    virtual std::vector<std::string> getSupportedTypes() const = 0;
    virtual std::unique_ptr<Plugin> createPlugin(const std::string& pluginName) = 0;
    virtual std::vector<std::string> getAvailablePlugins() const = 0;
    virtual bool isPluginAvailable(const std::string& pluginName) const = 0;
};

// Brain component plugin interface
class BrainComponentPlugin : public Plugin {
public:
    virtual ~BrainComponentPlugin() = default;
    
    PluginType getType() const override { return PluginType::BrainComponent; }
    
    // Component creation
    virtual std::string getComponentId() const = 0;
    virtual std::any createComponent(const std::map<std::string, std::any>& config) = 0;
    virtual bool configureComponent(const std::string& componentId, const std::map<std::string, std::any>& config) = 0;
    virtual bool destroyComponent(const std::string& componentId) = 0;
    
    // Component operations
    virtual bool activateComponent(const std::string& componentId) = 0;
    virtual bool deactivateComponent(const std::string& componentId) = 0;
    virtual bool isComponentActive(const std::string& componentId) const = 0;
    virtual std::vector<std::string> getActiveComponents() const = 0;
};

// Plasticity rule plugin interface
class PlasticityRulePlugin : public Plugin {
public:
    virtual ~PlasticityRulePlugin() = default;
    
    PluginType getType() const override { return PluginType::PlasticityRule; }
    
    // Plasticity rule definition
    virtual std::string getRuleId() const = 0;
    virtual float getLearningRate() const = 0;
    virtual float getBaselineDecay() const = 0;
    virtual bool requiresPrePostSynapticSpikes() const = 0;
    
    // Plasticity computation
    virtual float computeWeightChange(const std::map<std::string, std::any>& preSynapticInfo,
                                    const std::map<std::string, std::any>& postSynapticInfo,
                                    const std::map<std::string, std::any>& synapseInfo) = 0;
    virtual bool shouldApplyPlasticity(const std::map<std::string, std::any>& context) = 0;
};

// Environment plugin interface
class EnvironmentPlugin : public Plugin {
public:
    virtual ~EnvironmentPlugin() = default;
    
    PluginType getType() const override { return PluginType::Environment; }
    
    // Environment simulation
    virtual std::any getState() const = 0;
    virtual bool step(std::chrono::milliseconds duration) = 0;
    virtual void reset() = 0;
    
    // Sensory input/output
    virtual std::vector<float> getSensoryInput() const = 0;
    virtual void setMotorOutput(const std::vector<float>& output) = 0;
    virtual bool isEpisodeComplete() const = 0;
    
    // Environmental factors
    virtual float getTemperature() const = 0;
    virtual float getHumidity() const = 0;
    virtual float getLightLevel() const = 0;
    virtual float getNoiseLevel() const = 0;
};

// Memory system plugin interface
class MemorySystemPlugin : public Plugin {
public:
    virtual ~MemorySystemPlugin() = default;
    
    PluginType getType() const override { return PluginType::MemorySystem; }
    
    // Memory operations
    virtual std::string store(const std::map<std::string, std::any>& data) = 0;
    virtual std::any retrieve(const std::string& key) = 0;
    virtual bool contains(const std::string& key) const = 0;
    virtual void remove(const std::string& key) = 0;
    virtual void clear() = 0;
    
    // Memory management
    virtual size_t getMemoryUsage() const = 0;
    virtual size_t getCapacity() const = 0;
    virtual float getUtilizationRatio() const = 0;
};

// Cognition system plugin interface
class CognitionSystemPlugin : public Plugin {
public:
    virtual ~CognitionSystemPlugin() = default;
    
    PluginType getType() const override { return PluginType::CognitionSystem; }
    
    // Cognitive operations
    virtual std::vector<std::string> process(const std::vector<std::string>& input) = 0;
    virtual std::any getInternalState() const = 0;
    virtual void setInternalState(const std::any& state) = 0;
    
    // Cognitive capabilities
    virtual bool hasPlanning() const = 0;
    virtual bool hasAttention() const = 0;
    virtual bool hasConceptFormation() const = 0;
    virtual bool hasSocialLearning() const = 0;
};

// Neuromodulation plugin interface
class NeuromodulationPlugin : public Plugin {
public:
    virtual ~NeuromodulationPlugin() = default;
    
    PluginType getType() const override { return PluginType::Neuromodulation; }
    
    // Neuromodulator properties
    virtual std::string getModulatorId() const = 0;
    virtual float getBaselineLevel() const = 0;
    virtual float getMaxLevel() const = 0;
    virtual float getMinLevel() const = 0;
    
    // Neuromodulation effects
    virtual float getModulationFactor() const = 0;
    virtual void update(float deltaTime) = 0;
    virtual void applyToNeurons(std::vector<float>& neuronParameters) = 0;
};

// Experiment plugin interface
class ExperimentPlugin : public Plugin {
public:
    virtual ~ExperimentPlugin() = default;
    
    PluginType getType() const override { return PluginType::Experiment; }
    
    // Experiment definition
    virtual std::string getExperimentId() const = 0;
    virtual std::string getProtocol() const = 0;
    virtual std::chrono::milliseconds getDuration() const = 0;
    virtual size_t getMaxEpisodes() const = 0;
    
    // Experiment execution
    virtual std::any run(const std::map<std::string, std::any>& config) = 0;
    virtual bool canRun(const std::map<std::string, std::any>& config) const = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

// Utility plugin interface
class UtilityPlugin : public Plugin {
public:
    virtual ~UtilityPlugin() = default;
    
    PluginType getType() const override { return PluginType::Utility; }
    
    // Utility functions
    virtual std::any executeUtility(const std::string& utilityName,
                                   const std::map<std::string, std::any>& parameters) = 0;
    virtual std::vector<std::string> getAvailableUtilities() const = 0;
};

// Visualization plugin interface
class VisualizationPlugin : public Plugin {
public:
    virtual ~VisualizationPlugin() = default;
    
    PluginType getType() const override { return PluginType::Visualization; }
    
    // Visualization operations
    virtual bool initializeDisplay(int width, int height) = 0;
    virtual void render(const std::map<std::string, std::any>& data) = 0;
    virtual void update() = 0;
    virtual void shutdownDisplay() = 0;
    
    // Visualization properties
    virtual std::string getVisualizationType() const = 0;
    virtual bool supportsInteractive() const = 0;
    virtual std::vector<std::string> getVisualizationModes() const = 0;
};

// Statistics plugin interface
class StatisticsPlugin : public Plugin {
public:
    virtual ~StatisticsPlugin() = default;
    
    PluginType getType() const override { return PluginType::Statistics; }
    
    // Statistics collection
    virtual void recordMetric(const std::string& name, float value, 
                             const std::map<std::string, std::any>& tags = {}) = 0;
    virtual float getMetric(const std::string& name, const std::map<std::string, std::any>& filters = {}) const = 0;
    virtual std::vector<std::pair<std::string, float>> getAllMetrics() const = 0;
    
    // Statistics analysis
    virtual std::any computeStatistics(const std::string& metricName,
                                       const std::map<std::string, std::any>& options) = 0;
    virtual void reset() = 0;
};

// Plugin manager
class PluginManager {
public:
    PluginManager();
    ~PluginManager() = default;
    
    // Plugin loading and unloading
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool reloadPlugin(const std::string& pluginName);
    
    // Plugin management
    bool hasPlugin(const std::string& pluginName) const;
    void clearPlugins();
    
    // Plugin discovery and management
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getAvailablePlugins() const;
    Plugin* getPlugin(const std::string& pluginName);
    std::vector<Plugin*> getPluginsOfType(PluginType type) const;
    
    // Plugin configuration
    bool configurePlugin(const std::string& pluginName,
                        const std::map<std::string, std::any>& config);
    std::map<std::string, std::any> getPluginConfig(const std::string& pluginName) const;
    
    // Plugin factory management
    void registerFactory(std::unique_ptr<PluginFactory> factory);
    bool unregisterFactory(const std::string& factoryName);
    PluginFactory* getFactory(const std::string& factoryName);
    
    // Plugin validation
    bool validatePlugin(const std::string& pluginPath) const;
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;
    
    // Plugin statistics
    size_t getPluginCount() const { return loadedPlugins_.size(); }
    std::string getPluginVersion(const std::string& pluginName) const;
    std::string getPluginDescription(const std::string& pluginName) const;
    
private:
    std::map<std::string, std::unique_ptr<Plugin>> loadedPlugins_;
    std::map<std::string, std::unique_ptr<PluginFactory>> factories_;
    std::map<std::string, std::string> pluginPaths_;
    std::map<std::string, std::map<std::string, std::any>> pluginConfigs_;
    
    bool loadPluginFromFactory(const std::string& pluginName, const std::string& factoryName);
    bool loadPluginFromFile(const std::string& pluginPath);
    bool initializePlugin(Plugin& plugin, const std::map<std::string, std::any>& config);
};

// Plugin loader interface
class PluginLoader {
public:
    virtual ~PluginLoader() = default;
    
    virtual bool canLoad(const std::string& path) const = 0;
    virtual std::unique_ptr<Plugin> loadPlugin(const std::string& path) = 0;
    virtual std::vector<std::string> getSupportedExtensions() const = 0;
    virtual std::string getLoaderName() const = 0;
};

// File-based plugin loader
class FilePluginLoader : public PluginLoader {
public:
    FilePluginLoader();
    ~FilePluginLoader() override = default;
    
    bool canLoad(const std::string& path) const override;
    std::unique_ptr<Plugin> loadPlugin(const std::string& path) override;
    std::vector<std::string> getSupportedExtensions() const override;
    std::string getLoaderName() const override { return "FilePluginLoader"; }
    
private:
    bool loadSharedLibrary(const std::string& path);
    std::unique_ptr<Plugin> createPluginFromSymbol(const std::string& symbolName);
};

// Plugin configuration
class PluginConfig {
public:
    struct PluginEntry {
        std::string name;
        std::string path;
        std::map<std::string, std::any> config;
        bool autoLoad;
        bool enabled;
        
        PluginEntry() : name(), path(), autoLoad(true), enabled(true) {}
    };
    
    PluginConfig();
    ~PluginConfig() = default;
    
    // Configuration management
    void addPlugin(const PluginEntry& entry);
    bool removePlugin(const std::string& name);
    bool updatePlugin(const std::string& name, const PluginEntry& entry);
    
    // Configuration retrieval
    PluginEntry* getPlugin(const std::string& name);
    const PluginEntry* getPlugin(const std::string& name) const;
    bool hasPlugin(const std::string& name) const;
    std::vector<std::string> getPluginNames() const;
    
    // Configuration loading and saving
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool loadFromString(const std::string& content);
    std::string saveToString() const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    std::map<std::string, PluginEntry> plugins_;
    std::vector<std::string> validationErrors_;
    
    static std::string trim(const std::string& str);
    static bool isValidPluginName(const std::string& name);
    static bool isValidPath(const std::string& path);
};

// Default plugin implementations
// These provide basic plugins that can be extended by users

// Default brain component plugin
class DefaultBrainComponentPlugin : public BrainComponentPlugin {
public:
    std::string getName() const override { return "default_brain_component"; }
    std::string getVersion() const override { return NLM_PLUGIN_VERSION; }
    std::string getDescription() const override { 
        return "Default brain component plugin for NLM"; 
    }
    PluginType getType() const override { return PluginType::BrainComponent; }
    std::string getAuthor() const override { return "NLM Team"; }
    std::string getWebsite() const override { return "https://nlm.org"; }
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isInitialized() const override;
    
    std::vector<std::string> getDependencies() const override;
    std::vector<std::string> getOptionalDependencies() const override;
    
    bool canHandle(const std::string& key) const override;
    std::any execute(const std::string& operation, const std::map<std::string, std::any>& parameters) override;
    
    // BrainComponentPlugin specific methods
    std::string getComponentId() const override;
    std::any createComponent(const std::map<std::string, std::any>& config) override;
    bool configureComponent(const std::string& componentId, const std::map<std::string, std::any>& config) override;
    bool destroyComponent(const std::string& componentId) override;
    bool activateComponent(const std::string& componentId) override;
    bool deactivateComponent(const std::string& componentId) override;
    bool isComponentActive(const std::string& componentId) const override;
    std::vector<std::string> getActiveComponents() const override;
private:
    bool initialized_;
    std::map<std::string, std::any> components_;
};

// Default plasticity rule plugin
class DefaultPlasticityRulePlugin : public PlasticityRulePlugin {
public:
    std::string getName() const override { return "default_plasticity_rule"; }
    std::string getVersion() const override { return NLM_PLUGIN_VERSION; }
    std::string getDescription() const override { 
        return "Default plasticity rule plugin for NLM (STDP and Hebbian learning)"; 
    }
    PluginType getType() const override { return PluginType::PlasticityRule; }
    std::string getAuthor() const override { return "NLM Team"; }
    std::string getWebsite() const override { return "https://nlm.org"; }
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isInitialized() const override;
    
    std::vector<std::string> getDependencies() const override;
    std::vector<std::string> getOptionalDependencies() const override;
    
    bool canHandle(const std::string& key) const override;
    std::any execute(const std::string& operation, const std::map<std::string, std::any>& parameters) override;
    
    // PlasticityRulePlugin specific methods
    std::string getRuleId() const override;
    float getLearningRate() const override;
    float getBaselineDecay() const override;
    bool requiresPrePostSynapticSpikes() const override;
    float computeWeightChange(const std::map<std::string, std::any>& preSynapticInfo,
                             const std::map<std::string, std::any>& postSynapticInfo,
                             const std::map<std::string, std::any>& synapseInfo) override;
    bool shouldApplyPlasticity(const std::map<std::string, std::any>& context) override;
private:
    bool initialized_;
    std::string ruleId_;
    float learningRate_;
    float baselineDecay_;
    bool requiresSpikes_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_HPP