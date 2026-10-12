#pragma once

#include "NLMPlugin.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class NeuronPlugin
 * @brief Plugin interface for custom neuron types
 */
class NeuronPlugin : public virtual NLMPlugin {
public:
    virtual ~NeuronPlugin() = default;
    
    // Type check
    bool isNeuronPlugin() const override { return true; }
    
    // Neuron factory methods
    virtual void* getNeuronFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for neurons
    virtual Neuron* createNeuron(NeuronId id) = 0;
    virtual Neuron* createNeuron(NeuronId id, NeuronType type) = 0;
    virtual bool canCreateNeuron(const std::string& neuronType) const = 0;
    virtual std::vector<std::string> getSupportedNeuronTypes() const = 0;
    
    // Neuron configuration
    virtual bool configureNeuron(Neuron& neuron, const std::string& config) = 0;
    virtual std::string getNeuronConfig(const Neuron& neuron) const = 0;
};

} // namespace nlm