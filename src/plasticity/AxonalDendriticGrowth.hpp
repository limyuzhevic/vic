#pragma once

#include "StructuralPlasticity.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"

namespace nlm {

// Axonal/dendritic growth rules
// Structural growth of axons and dendrites based on activity and neurotrophic factors
class AxonalDendriticGrowth : public StructuralPlasticity {
public:
    AxonalDendriticGrowth();
    ~AxonalDendriticGrowth() override;

    SynapseId createSynapse(Brain* brain, NeuronId source, 
                            NeuronId destination, SynapticWeight weight) override;
    bool removeSynapse(Brain* brain, SynapseId synapse) override;
    bool removeNeuron(Brain* brain, NeuronId neuron) override;

    // Growth parameters
    void setAxonalGrowthRate(float rate);                   // Axonal growth rate (α)
    float getAxonalGrowthRate() const;
    void setDendriticGrowthRate(float rate);               // Dendritic growth rate (β)
    float getDendriticGrowthRate() const;
    void setNeurotrophicFactor(float factor);              // Neurotrophic factor (η)
    float getNeurotrophicFactor() const;
    void setMaxAxonalLength(float maxLength);              // Maximum axonal length
    float getMaxAxonalLength() const;
    void setMaxDendriticLength(float maxLength);           // Maximum dendritic length
    float getMaxDendriticLength() const;

    // Growth implementation
    void updateGrowth(Brain* brain, TimestepDuration dt);                  // Update growth
    float calculateAxonalGrowth(Brain* brain, NeuronId source, NeuronId destination) const; // Calculate axonal growth
    float calculateDendriticGrowth(Brain* brain, NeuronId source, NeuronId destination) const; // Calculate dendritic growth
    void updateAxonalLength(Brain* brain, NeuronId source, NeuronId destination, float growthAmount); // Update axonal length
    void updateDendriticLength(Brain* brain, NeuronId source, NeuronId destination, float growthAmount); // Update dendritic length
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm