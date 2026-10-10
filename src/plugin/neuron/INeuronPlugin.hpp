// Neuron plugin base class interface
#pragma once

#include <string>
#include <vector>
#include "../PluginBase.hpp"
#include "../brain/Neuron.hpp"

namespace nlm {

class INeuronPlugin : public IPlugin {
public:
    virtual ~INeuronPlugin() = default;
    
    // Plugin category identification
    virtual PluginCategory getCategory() const override { return PluginCategory::NEURON; }
    
    // Neuron creation and management
    virtual std::shared_ptr<Neuron> createNeuron(NeuronId id) = 0;
    virtual bool configureNeuron(Neuron& neuron, const std::map<std::string, std::string>& parameters) = 0;
    virtual std::vector<std::string> getNeuronTypes() const = 0;
    
    // Neuron dynamics customization
    virtual bool initializeNeuronDynamics(Neuron& neuron) = 0;
    virtual void updateNeuronState(Neuron& neuron, Timestamp currentTime, TimestepDuration dt) = 0;
    virtual bool onNeuronSpike(Neuron& neuron, Timestamp spikeTime) = 0;
    
    // Plugin-specific configuration
    virtual std::map<std::string, std::string> getDefaultParameters() const = 0;
    virtual std::vector<std::string> getSupportedParameterTypes() const = 0;
    
    // Neuron population management
    virtual bool createNeuronPopulation(std::vector<std::shared_ptr<Neuron>>& neurons, const std::vector<NeuronId>& ids) = 0;
    virtual bool configureNeuronPopulation(std::vector<std::shared_ptr<Neuron>>& neurons, const std::map<std::string, std::string>& parameters) = 0;
};

} // namespace nlm