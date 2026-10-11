#pragma once

#include "PluginBase.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

enum class NeuronType {
    LIF,
    ADAPTIVE,
    CUSTOM
};

enum class SynapseType {
    STANDARD,
    SPATIAL,
    DELAYED,
    CUSTOM
};

class NeuralNetworkPlugin : public NLMPlugin {
public:
    // Neural computation extension
    virtual void processNeurons(std::vector<Neuron*>& neurons, double deltaTime) = 0;
    virtual void processSynapses(std::vector<Synapse*>& synapses, double deltaTime) = 0;
    virtual void initializeNeurons(NeuralRegion* region) = 0;
    
    // Plugin-specific neural models
    virtual NeuronType getNeuronType() const = 0;
    virtual SynapseType getSynapseType() const = 0;
    virtual double getDefaultNeuronParameters() const = 0;
    virtual double getDefaultSynapseParameters() const = 0;
};

} // namespace nlm
