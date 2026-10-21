#pragma once

#include "StructuralPlasticity.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"

namespace nlm {

// Activity-dependent synapse formation
// Enhanced synaptogenesis that depends on neuronal activity patterns
class ActivityDependentSynaptogenesis : public StructuralPlasticity {
public:
    ActivityDependentSynaptogenesis();
    ~ActivityDependentSynaptogenesis() override;

    SynapseId createSynapse(Brain* brain, NeuronId source, 
                            NeuronId destination, SynapticWeight weight) override;
    bool removeSynapse(Brain* brain, SynapseId synapse) override;
    bool removeNeuron(Brain* brain, NeuronId neuron) override;

    // Activity-dependent parameters
    void setSynaptogenesisRate(float rate);                    // Base synaptogenesis rate
    float getSynaptogenesisRate() const;
    void setActivityThreshold(float threshold);                // Minimum activity for synapse formation
    float getActivityThreshold() const;
    void setCoactivityWindow(float window);                    // Time window for coactivity (ms)
    float getCoactivityWindow() const;
    void setMaxSynapsesPerPair(size_t max);                     // Maximum synapses per neuron pair
    size_t getMaxSynapsesPerPair() const;
    void setActivityDependencyWeight(float weight);             // Weight of activity dependence
    float getActivityDependencyWeight() const;

    // Activity-based implementation
    float calculateCoactivity(Brain* brain, NeuronId source, NeuronId destination) const;  // Calculate coactivity between neurons
    bool shouldCreateSynapse(Brain* brain, NeuronId source, NeuronId destination) const;     // Determine if synapse should be created
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm