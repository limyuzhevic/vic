// Example Custom Neuron Plugin
// Demonstrates creating a custom neuron model beyond LIF

#include "../include/nlm/plugin/neuron/NeuronPlugin.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/core/Types/Types.hpp"
#include <string>
#include <vector>

class CustomNeuronPlugin : public nlm::NeuronPlugin {
public:
    NLM_PLUGIN_IMPLEMENTATION
    
    // Custom neuron factory
    nlm::Neuron* createNeuron(nlm::NeuronId id) override {
        return new nlm::Neuron(id);
    }
    
    nlm::Neuron* createNeuron(nlm::NeuronId id, nlm::NeuronType type) override {
        nlm::Neuron* neuron = new nlm::Neuron(id);
        neuron->setType(type);
        return neuron;
    }
    
    bool canCreateNeuron(const std::string& neuronType) const override {
        return neuronType == "lif" || neuronType == "adaptive_lif" || 
               neuronType == "gaussian" || neuronType == "bh" || neuronType == "ijng";
    }
    
    std::vector<std::string> getSupportedNeuronTypes() const override {
        return {"lif", "adaptive_lif", "gaussian", "bh", "ijng", "spiking_granular", "reservoir"};
    }
    
    bool configureNeuron(nlm::Neuron& neuron, const std::string& config) override {
        // Parse configuration and apply to neuron
        if (config.find("threshold=") != std::string::npos) {
            // Extract threshold and configure neuron
        }
        return true;
    }
    
    std::string getNeuronConfig(const nlm::Neuron& neuron) const override {
        return "custom_neuron_config:id=" + std::to_string(neuron.getId().index());
    }
};

NLM_PLUGIN_FACTORY(CustomNeuronPlugin)