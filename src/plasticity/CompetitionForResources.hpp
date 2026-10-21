#pragma once

#include "StructuralPlasticity.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"

namespace nlm {

// Competition for resources
// Structural competition for synaptic resources to maintain balance
class CompetitionForResources : public StructuralPlasticity {
public:
    CompetitionForResources();
    ~CompetitionForResources() override;

    SynapseId createSynapse(Brain* brain, NeuronId source, 
                            NeuronId destination, SynapticWeight weight) override;
    bool removeSynapse(Brain* brain, SynapseId synapse) override;
    bool removeNeuron(Brain* brain, NeuronId neuron) override;

    // Competition parameters
    void setCompetitionIntensity(float intensity);            // Competition intensity (α)
    float getCompetitionIntensity() const;
    void setResourcePoolSize(size_t poolSize);               // Maximum available resources
    size_t getResourcePoolSize() const;
    void setPruningThreshold(float threshold);               // Threshold for resource-based pruning
    float getPruningThreshold() const;
    void setResourceAllocationStrategy(int strategy);        // Resource allocation strategy
    int getResourceAllocationStrategy() const;

    // Competition implementation
    void updateResourceAllocation(Brain* brain);                    // Update resource allocation
    float calculateResourceDemand(Brain* brain) const;              // Calculate total resource demand
    bool hasEnoughResources(Brain* brain, size_t additionalSynapses) const; // Check if there are enough resources
    size_t selectSynapsesForPruning(Brain* brain, size_t count) const;  // Select synapses to prune based on competition
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm