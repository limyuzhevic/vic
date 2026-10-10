#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <algorithm>

namespace nlm {

// Plugin system version
#define NLM_PLUGIN_VERSION_MAJOR 1
#define NLM_PLUGIN_VERSION_MINOR 0

// Plugin types
enum class PluginType {
    NeuronModel,
    PlasticityRule,
    Neuromodulator,
    Sensor,
    Action,
    MemorySystem,
    CognitiveSystem
};

// Plugin capabilities
struct PluginCapabilities {
    PluginType type;
    std::string name;
    std::string description;
    std::string version;
    std::vector<std::string> dependencies;
    bool isCorePlugin = false;
};

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string license;
    PluginType type;
    bool enabled = true;
    std::vector<std::string> tags;
};

// Plugin interface - base class for all plugins
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Get plugin metadata
    virtual const PluginMetadata& getMetadata() const = 0;
    
    // Initialize plugin
    virtual bool initialize() = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Check if plugin is initialized
    virtual bool isInitialized() const = 0;
    
    // Get plugin capabilities
    virtual const PluginCapabilities& getCapabilities() const = 0;
};

// Neuron model plugin interface
class NeuronModelPlugin : public PluginInterface {
public:
    virtual ~NeuronModelPlugin() = default;
    
    // Create a new neuron instance
    virtual std::unique_ptr<class Neuron> createNeuron(NeuronId id) = 0;
    
    // Get neuron model parameters
    virtual const std::unordered_map<std::string, float>& getDefaultParameters() const = 0;
    
    // Validate parameters
    virtual bool validateParameters(const std::unordered_map<std::string, float>& params) const = 0;
    
    // Get neuron model name
    virtual std::string getModelName() const = 0;
};

// Plasticity rule plugin interface
class PlasticityRulePlugin : public PluginInterface {
public:
    virtual ~PlasticityRulePlugin() = default;
    
    // Update synaptic weight based on pre- and post-synaptic spikes
    virtual void updateSynapticWeight(
        float& weight,
        const std::vector<Timestamp>& preSpikeHistory,
        const std::vector<Timestamp>& postSpikeHistory,
        float learningRate,
        float timeConstant
    ) = 0;
    
    // Get rule name
    virtual std::string getRuleName() const = 0;
    
    // Get rule parameters
    virtual const std::unordered_map<std::string, float>& getParameters() const = 0;
};

// Plugin factory interface
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    
    // Create a plugin instance from metadata
    virtual std::unique_ptr<PluginInterface> createPlugin(const PluginMetadata& metadata) = 0;
    
    // Check if this factory can create the given plugin
    virtual bool canCreate(const PluginMetadata& metadata) const = 0;
};

// Plugin registry - manages all available plugins
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Register a plugin factory
    void registerFactory(std::unique_ptr<PluginFactory> factory);
    
    // Register a core plugin (built-in)
    void registerCorePlugin(std::unique_ptr<PluginInterface> plugin);
    
    // Load a plugin from file (dynamic loading)
    bool loadPlugin(const std::string& filepath);
    
    // Unload a plugin
    bool unloadPlugin(const std::string& pluginName);
    
    // Get plugin by name
    std::unique_ptr<PluginInterface> getPlugin(const std::string& name);
    
    // Get all available plugins
    std::vector<std::unique_ptr<PluginInterface>> getAllPlugins() const;
    
    // Get plugins by type
    std::vector<std::unique_ptr<PluginInterface>> getPluginsByType(PluginType type) const;
    
    // Enable/disable plugin
    void setPluginEnabled(const std::string& name, bool enabled);
    
    // Check if plugin is enabled
    bool isPluginEnabled(const std::string& name) const;
    
    // Initialize all enabled plugins
    bool initializeAllPlugins();
    
    // Shutdown all plugins
    void shutdownAllPlugins();
    
    // Get built-in neuron model plugins
    std::vector<std::unique_ptr<NeuronModelPlugin>> getNeuronModels() const;
    
    // Get built-in plasticity rule plugins
    std::vector<std::unique_ptr<PlasticityRulePlugin>> getPlasticityRules() const;
    
    // Create a neuron using registered neuron model
    std::unique_ptr<class Neuron> createNeuron(const std::string& modelName, NeuronId id);
    
    // Apply a plasticity rule to a synapse
    void applyPlasticityRule(
        std::unique_ptr<class Synapse>& synapse,
        const std::string& ruleName,
        const std::vector<Timestamp>& preSpikeHistory,
        const std::vector<Timestamp>& postSpikeHistory,
        float learningRate,
        float timeConstant
    );
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin configuration
struct PluginConfig {
    std::string name;
    std::unordered_map<std::string, float> parameters;
    std::unordered_map<std::string, std::string> stringParameters;
    bool enabled = true;
    std::string filepath; // For dynamic plugins
};

// Built-in plugin registration macros
#define REGISTER_NEURON_MODEL(ModelName, ModelClass) \
    namespace { \
        class ModelName##_PluginRegistrar { \
        public: \
            ModelName##_PluginRegistrar() { \
                PluginRegistry::instance().registerFactory( \
                    std::make_unique<ModelName##_PluginFactory>() \
                ); \
            } \
        }; \
        ModelName##_PluginRegistrar ModelName##_RegistrarInstance; \
    }

#define REGISTER_PLASTICITY_RULE(RuleName, RuleClass) \
    namespace { \
        class RuleName##_PluginRegistrar { \
        public: \
            RuleName##_PluginRegistrar() { \
                PluginRegistry::instance().registerFactory( \
                    std::make_unique<RuleName##_PluginFactory>() \
                ); \
            } \
        }; \
        RuleName##_PluginRegistrar RuleName##_RegistrarInstance; \
    }

// Global registry instance
inline PluginRegistry& pluginRegistry() {
    static PluginRegistry registry;
    return registry;
}

} // namespace nlm
