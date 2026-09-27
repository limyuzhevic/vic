#ifndef NLM_PLUGIN_BRAINPLUGINSYSTEM_HPP
#define NLM_PLUGIN_BRAINPLUGINSYSTEM_HPP

#include "PluginBase.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace plugin {

// Forward declarations
class Brain;
class Neuron;
class Synapse;
class IMemorySystem;

// Brain-specific plugin types
class AbstractNeuronPlugin : public virtual IPlugin, public virtual AbstractComponentPlugin {
public:
    virtual ~AbstractNeuronPlugin() = default;
    
    virtual std::string getNeuronType() const = 0;
    virtual std::string getNeuronName() const = 0;
    
    virtual bool configureNeuron(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual std::unordered_map<std::string, std::string> getNeuronConfig() const = 0;
    
    virtual Neuron* createNeuron(const std::string& name) = 0;
    virtual bool destroyNeuron(Neuron* neuron) = 0;
    virtual std::vector<Neuron*> getNeurons() const = 0;
    
    virtual bool preNeuronInitialization(Neuron* neuron) = 0;
    virtual bool postNeuronInitialization(Neuron* neuron) = 0;
    virtual bool preNeuronShutdown(Neuron* neuron) = 0;
    virtual bool postNeuronShutdown(Neuron* neuron) = 0;
};

class AbstractSynapsePlugin : public virtual IPlugin, public virtual AbstractComponentPlugin {
public:
    virtual ~AbstractSynapsePlugin() = default;
    
    virtual std::string getSynapseType() const = 0;
    virtual std::string getSynapseName() const = 0;
    
    virtual bool configureSynapse(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual std::unordered_map<std::string, std::string> getSynapseConfig() const = 0;
    
    virtual Synapse* createSynapse(const std::string& name) = 0;
    virtual bool destroySynapse(Synapse* synapse) = 0;
    virtual std::vector<Synapse*> getSynapses() const = 0;
    
    virtual bool preSynapseInitialization(Synapse* synapse) = 0;
    virtual bool postSynapseInitialization(Synapse* synapse) = 0;
    virtual bool preSynapseShutdown(Synapse* synapse) = 0;
    virtual bool postSynapseShutdown(Synapse* synapse) = 0;
    
    virtual bool connectSynapse(Neuron* preNeuron, Neuron* postNeuron) = 0;
    virtual bool disconnectSynapse(Neuron* preNeuron, Neuron* postNeuron) = 0;
};

class AbstractMemoryPlugin : public virtual IPlugin, public virtual AbstractComponentPlugin {
public:
    virtual ~AbstractMemoryPlugin() = default;
    
    virtual std::string getMemoryType() const = 0;
    virtual std::string getMemoryName() const = 0;
    
    virtual bool configureMemory(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual std::unordered_map<std::string, std::string> getMemoryConfig() const = 0;
    
    virtual IMemorySystem* createMemorySystem(const std::string& name) = 0;
    virtual bool destroyMemorySystem(IMemorySystem* memory) = 0;
    virtual std::vector<IMemorySystem*> getMemorySystems() const = 0;
    
    virtual bool preMemorySystemInitialization(IMemorySystem* memory) = 0;
    virtual bool postMemorySystemInitialization(IMemorySystem* memory) = 0;
    virtual bool preMemorySystemShutdown(IMemorySystem* memory) = 0;
    virtual bool postMemorySystemShutdown(IMemorySystem* memory) = 0;
};

class BrainPluginSystem {
public:
    BrainPluginSystem();
    ~BrainPluginSystem();
    
    // Plugin registration
    bool registerNeuronPlugin(std::unique_ptr<AbstractNeuronPlugin> plugin);
    bool registerSynapsePlugin(std::unique_ptr<AbstractSynapsePlugin> plugin);
    bool registerMemoryPlugin(std::unique_ptr<AbstractMemoryPlugin> plugin);
    
    // Plugin management
    bool unregisterNeuronPlugin(const std::string& name);
    bool unregisterSynapsePlugin(const std::string& name);
    bool unregisterMemoryPlugin(const std::string& name);
    
    // Plugin access
    AbstractNeuronPlugin* getNeuronPlugin(const std::string& name) const;
    AbstractSynapsePlugin* getSynapsePlugin(const std::string& name) const;
    AbstractMemoryPlugin* getMemoryPlugin(const std::string& name) const;
    
    // Plugin lifecycle
    bool initializePlugins();
    bool activatePlugins();
    bool shutdownPlugins();
    bool unloadPlugins();
    
    // Plugin creation
    Neuron* createNeuron(const std::string& neuronType, const std::string& name);
    Synapse* createSynapse(const std::string& synapseType, const std::string& name);
    IMemorySystem* createMemorySystem(const std::string& memoryType, const std::string& name);
    
    // Plugin hooks
    void onNeuronCreated(Neuron* neuron);
    void onSynapseCreated(Synapse* synapse);
    void onMemorySystemInitialized(IMemorySystem* memory);
    void onNeuronDestroyed(Neuron* neuron);
    void onSynapseDestroyed(Synapse* synapse);
    void onMemorySystemDestroyed(IMemorySystem* memory);
    
    // Plugin integration with Brain
    bool integrateWithBrain(Brain* brain);
    bool deintegrateFromBrain(Brain* brain);
    bool isIntegratedWithBrain(Brain* brain) const;
    
    // Plugin configuration
    bool configurePlugin(const std::string& pluginType, const std::string& pluginName, const std::unordered_map<std::string, std::string>& config);
    std::unordered_map<std::string, std::string> getPluginConfig(const std::string& pluginType, const std::string& pluginName) const;
    
    // Plugin statistics
    struct PluginStats {
        std::string name;
        std::string type;
        PluginState state;
        uint64_t instanceCount = 0;
        uint64_t errorCount = 0;
        std::string lastError;
        std::chrono::duration<double> averageCreationTime;
    };
    
    std::vector<PluginStats> getAllPluginStats() const;
    PluginStats getPluginStats(const std::string& pluginType, const std::string& pluginName) const;
    
    // Plugin monitoring
    void onPluginStateChanged(const std::string& pluginType, const std::string& pluginName, PluginState oldState, PluginState newState);
    void onPluginError(const std::string& pluginType, const std::string& pluginName, const std::string& error);
    
    // Plugin validation
    bool validatePluginConfiguration(const std::string& pluginType, const std::string& pluginName) const;
    std::vector<std::string> getPluginValidationErrors(const std::string& pluginType, const std::string& pluginName) const;
    
    // Plugin composition
    bool composePlugins(const std::vector<std::string>& pluginNames, const std::string& composedPluginName);
    bool decomposePlugin(const std::string& composedPluginName, std::vector<std::string>& componentPluginNames);
    
    // Plugin performance
    double getAverageNeuronCreationTime() const;
    double getAverageSynapseCreationTime() const;
    double getAverageMemorySystemCreationTime() const;
    
    // Plugin debugging
    std::string getPluginDebugInfo(const std::string& pluginType, const std::string& pluginName) const;
    void dumpPluginState(const std::string& filename) const;
    
private:
    struct PluginContainer {
        std::unique_ptr<AbstractNeuronPlugin> neuronPlugin;
        std::unique_ptr<AbstractSynapsePlugin> synapsePlugin;
        std::unique_ptr<AbstractMemoryPlugin> memoryPlugin;
    };
    
    mutable std::shared_mutex mutex_;
    PluginContainer plugins_;
    
    std::unordered_map<std::string, Neuron*> neuronInstances_;
    std::unordered_map<std::string, Synapse*> synapseInstances_;
    std::unordered_map<std::string, IMemorySystem*> memoryInstances_;
    
    std::vector<std::function<void(const std::string&, const std::string&, PluginState, PluginState)>> stateChangeCallbacks_;
    std::vector<std::function<void(const std::string&, const std::string&, const std::string&)>> errorCallbacks_;
    
    bool validatePluginCompatibility(const AbstractNeuronPlugin& neuronPlugin) const;
    bool validatePluginCompatibility(const AbstractSynapsePlugin& synapsePlugin) const;
    bool validatePluginCompatibility(const AbstractMemoryPlugin& memoryPlugin) const;
    
    void updatePluginStats(const std::string& pluginType, const std::string& pluginName, PluginState state);
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_BRAINPLUGINSYSTEM_HPP