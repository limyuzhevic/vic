// Neural Component Factory and Extension System
// Provides factory patterns and extension points for creating and customizing neural components

#ifndef NLM_EXTENSION_SYSTEM_HPP
#define NLM_EXTENSION_SYSTEM_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>

namespace nlm {

// Extension point interfaces
class NeuronFactory {
public:
    virtual ~NeuronFactory() = default;
    virtual std::shared_ptr<Neuron> createNeuron(NeuronId id, NeuronType type) = 0;
    virtual std::string getName() const = 0;
};

class SynapseFactory {
public:
    virtual ~SynapseFactory() = default;
    virtual std::shared_ptr<Synapse> createSynapse(NeuronId preId, NeuronId postId, 
                                                   double weight, double delay) = 0;
    virtual std::string getName() const = 0;
};

class PlasticityRuleFactory {
public:
    virtual ~PlasticityRuleFactory() = default;
    virtual std::shared_ptr<PlasticityRule> createPlasticityRule(const ConfigValue& params) = 0;
    virtual std::string getName() const = 0;
};

class MemorySystemFactory {
public:
    virtual ~MemorySystemFactory() = default;
    virtual std::shared_ptr<MemorySystem> createMemorySystem(const Config& config) = 0;
    virtual std::string getName() const = 0;
};

class NeuromodulatorFactory {
public:
    virtual ~NeuromodulatorFactory() = default;
    virtual std::shared_ptr<Neuromodulator> createNeuromodulator(const ConfigValue& params) = 0;
    virtual std::string getName() const = 0;
};

// Extension point registration system
class ExtensionRegistry {
public:
    ExtensionRegistry() = default;
    
    // Neuron factory management
    void registerNeuronFactory(std::unique_ptr<NeuronFactory> factory);
    std::vector<std::shared_ptr<NeuronFactory>> getNeuronFactories() const;
    
    // Synapse factory management
    void registerSynapseFactory(std::unique_ptr<SynapseFactory> factory);
    std::vector<std::shared_ptr<SynapseFactory>> getSynapseFactories() const;
    
    // Plasticity rule factory management
    void registerPlasticityRuleFactory(std::unique_ptr<PlasticityRuleFactory> factory);
    std::vector<std::shared_ptr<PlasticityRuleFactory>> getPlasticityRuleFactories() const;
    
    // Memory system factory management
    void registerMemorySystemFactory(std::unique_ptr<MemorySystemFactory> factory);
    std::vector<std::shared_ptr<MemorySystemFactory>> getMemorySystemFactories() const;
    
    // Neuromodulator factory management
    void registerNeuromodulatorFactory(std::unique_ptr<NeuromodulatorFactory> factory);
    std::vector<std::shared_ptr<NeuromodulatorFactory>> getNeuromodulatorFactories() const;
    
    // Configuration extension points
    void registerConfigExtension(const std::string& key, 
                                 std::function<ConfigValue(const Config&)> extensionFunc);
    std::map<std::string, std::function<ConfigValue(const Config&)>> getConfigExtensions() const;
    
    // Plugin loading
    bool loadPlugin(const std::string& pluginPath);
    
    // Extension point queries
    bool hasNeuronFactory(const std::string& name) const;
    bool hasSynapseFactory(const std::string& name) const;
    bool hasPlasticityRuleFactory(const std::string& name) const;
    bool hasMemorySystemFactory(const std::string& name) const;
    bool hasNeuromodulatorFactory(const std::string& name) const;
    
    // Extension point configuration
    void configureExtension(const std::string& name, const ConfigValue& config);
    
private:
    std::vector<std::unique_ptr<NeuronFactory>> neuronFactories;
    std::vector<std::unique_ptr<SynapseFactory>> synapseFactories;
    std::vector<std::unique_ptr<PlasticityRuleFactory>> plasticityRuleFactories;
    std::vector<std::unique_ptr<MemorySystemFactory>> memorySystemFactories;
    std::vector<std::unique_ptr<NeuromodulatorFactory>> neuromodulatorFactories;
    
    std::map<std::string, std::function<ConfigValue(const Config&)>> configExtensions;
    std::map<std::string, ConfigValue> extensionConfigs;
};

// Default factory implementations
class DefaultNeuronFactory : public NeuronFactory {
public:
    std::shared_ptr<Neuron> createNeuron(NeuronId id, NeuronType type) override {
        return std::make_shared<Neuron>(id, type);
    }
    
    std::string getName() const override { return "DefaultNeuronFactory"; }
};

class DefaultSynapseFactory : public SynapseFactory {
public:
    std::shared_ptr<Synapse> createSynapse(NeuronId preId, NeuronId postId, 
                                           double weight, double delay) override {
        return std::make_shared<Synapse>(preId, postId, weight, delay);
    }
    
    std::string getName() const override { return "DefaultSynapseFactory"; }
};

class DefaultPlasticityRuleFactory : public PlasticityRuleFactory {
public:
    std::shared_ptr<PlasticityRule> createPlasticityRule(const ConfigValue& params) override {
        // Create appropriate plasticity rule based on params
        // This is a simplified implementation
        return std::make_shared<STDP>(0.01, 0.012, 20.0);
    }
    
    std::string getName() const override { return "DefaultPlasticityRuleFactory"; }
};

class DefaultMemorySystemFactory : public MemorySystemFactory {
public:
    std::shared_ptr<MemorySystem> createMemorySystem(const Config& config) override {
        return std::make_shared<NeuralWorkingMemory>();
    }
    
    std::string getName() const override { return "DefaultMemorySystemFactory"; }
};

class DefaultNeuromodulatorFactory : public NeuromodulatorFactory {
public:
    std::shared_ptr<Neuromodulator> createNeuromodulator(const ConfigValue& params) override {
        // Create appropriate neuromodulator based on params
        return std::make_shared<Curiosity>(0.1, 0.9, 0.5);
    }
    
    std::string getName() const override { return "DefaultNeuromodulatorFactory"; }
};

// Brain factory with extension support
class ExtendedBrainFactory {
public:
    ExtendedBrainFactory(std::shared_ptr<ExtensionRegistry> registry = nullptr);
    
    std::shared_ptr<Brain> createBrain(const Config& config);
    
    // Configuration extension integration
    void integrateConfigExtensions(Config& config) const;
    
    // Component creation with factories
    std::shared_ptr<Neuron> createNeuron(NeuronId id, NeuronType type) const;
    std::shared_ptr<Synapse> createSynapse(NeuronId preId, NeuronId postId, 
                                           double weight, double delay) const;
    std::shared_ptr<PlasticityRule> createPlasticityRule(const ConfigValue& params) const;
    std::shared_ptr<MemorySystem> createMemorySystem(const Config& config) const;
    std::shared_ptr<Neuromodulator> createNeuromodulator(const ConfigValue& params) const;
    
private:
    std::shared_ptr<ExtensionRegistry> registry;
    
    // Default factories
    std::shared_ptr<NeuronFactory> defaultNeuronFactory;
    std::shared_ptr<SynapseFactory> defaultSynapseFactory;
    std::shared_ptr<PlasticityRuleFactory> defaultPlasticityRuleFactory;
    std::shared_ptr<MemorySystemFactory> defaultMemorySystemFactory;
    std::shared_ptr<NeuromodulatorFactory> defaultNeuromodulatorFactory;
    
    // Factory selection logic
    std::shared_ptr<NeuronFactory> selectNeuronFactory(const Config& config) const;
    std::shared_ptr<SynapseFactory> selectSynapseFactory(const Config& config) const;
    std::shared_ptr<PlasticityRuleFactory> selectPlasticityRuleFactory(const Config& config) const;
    std::shared_ptr<MemorySystemFactory> selectMemorySystemFactory(const Config& config) const;
    std::shared_ptr<NeuromodulatorFactory> selectNeuromodulatorFactory(const Config& config) const;
};

// Plugin system
class Plugin {
public:
    virtual ~Plugin() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
};

class PluginManager {
public:
    PluginManager(std::shared_ptr<ExtensionRegistry> registry);
    
    // Plugin management
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    
    // Plugin queries
    std::vector<std::string> getPluginNames() const;
    std::vector<std::string> getEnabledPluginNames() const;
    bool hasPlugin(const std::string& pluginName) const;
    bool isPluginEnabled(const std::string& pluginName) const;
    
    // Plugin configuration
    void configurePlugin(const std::string& pluginName, const ConfigValue& config);
    ConfigValue getPluginConfig(const std::string& pluginName) const;
    
    // Plugin execution
    void initializeAllPlugins(const Config& config);
    void shutdownAllPlugins();
    
    // Plugin notification
    void notifyPlugins(const std::string& event, const ConfigValue& data);
    
private:
    std::shared_ptr<ExtensionRegistry> registry;
    std::map<std::string, std::unique_ptr<Plugin>> plugins;
    std::map<std::string, bool> pluginEnabled;
    std::map<std::string, ConfigValue> pluginConfigs;
};

// Component builder with fluent interface
class NeuralComponentBuilder {
public:
    NeuralComponentBuilder() = default;
    
    // Builder methods
    NeuralComponentBuilder& withNeuronFactory(std::unique_ptr<NeuronFactory> factory);
    NeuralComponentBuilder& withSynapseFactory(std::unique_ptr<SynapseFactory> factory);
    NeuralComponentBuilder& withPlasticityRuleFactory(std::unique_ptr<PlasticityRuleFactory> factory);
    NeuralComponentBuilder& withMemorySystemFactory(std::unique_ptr<MemorySystemFactory> factory);
    NeuralComponentBuilder& withNeuromodulatorFactory(std::unique_ptr<NeuromodulatorFactory> factory);
    
    // Configuration
    NeuralComponentBuilder& withConfig(const Config& config);
    NeuralComponentBuilder& addConfigExtension(const std::string& key, 
                                               std::function<ConfigValue(const Config&)> extensionFunc);
    
    // Plugin management
    NeuralComponentBuilder& addPlugin(std::unique_ptr<Plugin> plugin);
    NeuralComponentBuilder& enablePlugin(const std::string& pluginName);
    
    // Build methods
    std::shared_ptr<Brain> buildBrain();
    ExtendedBrainFactory buildExtendedFactory();
    
    // Reset builder state
    void reset();
    
    // Builder state
    struct BuilderState {
        std::shared_ptr<Config> config;
        std::vector<std::unique_ptr<NeuronFactory>> neuronFactories;
        std::vector<std::unique_ptr<SynapseFactory>> synapseFactories;
        std::vector<std::unique_ptr<PlasticityRuleFactory>> plasticityRuleFactories;
        std::vector<std::unique_ptr<MemorySystemFactory>> memorySystemFactories;
        std::vector<std::unique_ptr<NeuromodulatorFactory>> neuromodulatorFactories;
        std::vector<std::unique_ptr<Plugin>> plugins;
        std::map<std::string, bool> enabledPlugins;
        std::map<std::string, ConfigValue> configExtensions;
    } state;
};

// Convenience functions for common extension patterns
namespace Extensions {
    // Standard extension sets
    void registerDefaultExtensions(ExtensionRegistry& registry);
    
    // Custom factory registration
    template<typename T, typename... Args>
    void registerFactory(ExtensionRegistry& registry, const std::string& name);
    
    // Extension configuration helpers
    void configureFromFile(ExtensionRegistry& registry, const std::string& configFile);
    void configureFromConfig(ExtensionRegistry& registry, const Config& config);
    
    // Plugin loading helpers
    bool loadBuiltInPlugins(ExtensionRegistry& registry);
    bool loadExternalPlugins(ExtensionRegistry& registry, const std::vector<std::string>& paths);
}

} // namespace nlm

#endif // NLM_EXTENSION_SYSTEM_HPP