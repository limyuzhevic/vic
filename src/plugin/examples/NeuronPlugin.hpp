#ifndef NLM_PLUGIN_EXAMPLES_NEURONPLUGIN_HPP
#define NLM_PLUGIN_EXAMPLES_NEURONPLUGIN_HPP

#include "AbstractComponentPlugin.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {
namespace plugin {

class NeuronPlugin : public AbstractComponentPlugin {
public:
    NeuronPlugin();
    ~NeuronPlugin() override;
    
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
    
    // Neuron-specific functionality
    class Neuron {
    public:
        Neuron(const std::string& name, NeuronPlugin* plugin);
        ~Neuron();
        
        std::string getName() const { return name_; }
        double getMembranePotential() const { return membranePotential_; }
        double getRefractoryPeriod() const { return refractoryPeriod_; }
        
        void setMembranePotential(double potential) { membranePotential_ = potential; }
        void setRefractoryPeriod(double period) { refractoryPeriod_ = period; }
        
        void update(double dt);
        void injectCurrent(double current);
        void reset();
        
    private:
        std::string name_;
        double membranePotential_;
        double refractoryPeriod_;
        double timeSinceSpike_;
        NeuronPlugin* plugin_;
    };
    
    Neuron* createNeuron(const std::string& name);
    bool destroyNeuron(Neuron* neuron);
    std::vector<Neuron*> getNeurons() const;
    Neuron* getNeuron(const std::string& name) const;
    
    // Neuron plugin configuration
    struct NeuronConfig {
        double restingPotential = -65.0;
        double thresholdPotential = -55.0;
        double capacitance = 1.0;
        double membraneResistance = 100.0;
        double timeConstant = 10.0;
        double refractoryPeriod = 2.0;
        double synapticStrength = 1.0;
    };
    
    NeuronConfig getNeuronConfig() const { return config_; }
    void setNeuronConfig(const NeuronConfig& config) { config_ = config; }
    
private:
    PluginMetadata metadata_;
    PluginState state_;
    std::string error_;
    NeuronConfig config_;
    std::vector<Neuron*> neurons_;
    std::unordered_map<std::string, void*> extensions_;
    std::unordered_map<std::string, std::string> componentConnections_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_EXAMPLES_NEURONPLUGIN_HPP