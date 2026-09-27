#ifndef NLM_PLUGIN_EXAMPLES_SYNAPSEPLUGIN_HPP
#define NLM_PLUGIN_EXAMPLES_SYNAPSEPLUGIN_HPP

#include "AbstractComponentPlugin.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace nlm {
namespace plugin {

class SynapsePlugin : public AbstractComponentPlugin {
public:
    SynapsePlugin();
    ~SynapsePlugin() override;
    
    // IPlugin interface implementation
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    PluginType getType() const override;
    PluginMetadata getMetadata() const override;
    
    bool initialize(const PluginMetadata& metadata) override;
    void shutdown() override;
    bool activate() override;
    bool deactivate() override;
    bool loadDependencies() override;
    bool checkConflicts() override;
    
    // AbstractComponentPlugin interface implementation
    std::string getComponentType() const override;
    std::string getComponentName() const override;
    
    bool configureComponent(const std::unordered_map<std::string, std::string>& config) override;
    std::unordered_map<std::string, std::string> getComponentConfig() const override;
    
    bool preInitialize() override;
    bool postInitialize() override;
    bool preShutdown() override;
    bool postShutdown() override;
    
    ComponentState getComponentState() const override;
    std::string getComponentStatus() const override;
    
    void* getComponentExtension(const std::string& extensionName) override;
    bool registerComponentExtension(const std::string& extensionName, void* extension) override;
    
    bool connectToComponent(const std::string& componentName) override;
    bool disconnectFromComponent(const std::string& componentName) override;
    bool isConnectedToComponent(const std::string& componentName) const override;
    
    bool validateComponentConfiguration() const override;
    bool validateComponentDependencies() const override;
    
    // Synapse-specific functionality
    class Synapse {
    public:
        Synapse(const std::string& name, SynapsePlugin* plugin);
        ~Synapse();
        
        std::string getName() const { return name_; }
        double getStrength() const { return strength_; }
        double getDelay() const { return delay_; }
        std::string getPreNeuronName() const { return preNeuronName_; }
        std::string getPostNeuronName() const { return postNeuronName_; }
        
        void setStrength(double strength) { strength_ = strength; }
        void setDelay(double delay) { delay_ = delay; }
        void setPreNeuron(const std::string& name) { preNeuronName_ = name; }
        void setPostNeuron(const std::string& name) { postNeuronName_ = name; }
        
        void update(double dt);
        bool connect(const std::string& preNeuronName, const std::string& postNeuronName);
        void disconnect();
        
    private:
        std::string name_;
        double strength_;
        double delay_;
        std::string preNeuronName_;
        std::string postNeuronName_;
        SynapsePlugin* plugin_;
    };
    
    Synapse* createSynapse(const std::string& name);
    bool destroySynapse(Synapse* synapse);
    std::vector<Synapse*> getSynapses() const;
    Synapse* getSynapse(const std::string& name) const;
    
    // Connect synapses between neurons
    bool connectSynapse(const std::string& synapseName, const std::string& preNeuronName, const std::string& postNeuronName);
    bool disconnectSynapse(const std::string& synapseName);
    std::vector<Synapse*> getSynapsesToNeuron(const std::string& neuronName) const;
    
    // Synapse plugin configuration
    struct SynapseConfig {
        double defaultStrength = 1.0;
        double defaultDelay = 1.0;
        double maxStrength = 10.0;
        double minStrength = 0.0;
        bool enableShortTermPlasticity = false;
        bool enableLongTermPlasticity = false;
        double plasticityRate = 0.01;
    };
    
    SynapseConfig getSynapseConfig() const { return config_; }
    void setSynapseConfig(const SynapseConfig& config) { config_ = config; }
    
private:
    PluginMetadata metadata_;
    PluginState state_;
    std::string error_;
    SynapseConfig config_;
    std::vector<Synapse*> synapses_;
    std::unordered_map<std::string, void*> extensions_;
    std::unordered_map<std::string, std::string> componentConnections_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_EXAMPLES_SYNAPSEPLUGIN_HPP