#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Synaptogenesis: formation of new synapses
// Enhanced implementation with activity-dependent dynamics, developmental regulation,
// and homeostatic balance for Phase 2 structural plasticity

class Synaptogenesis {
public:
    Synaptogenesis();
    ~Synaptogenesis();
    
    // Get formation rate
    float getFormationRate() const;
    void setFormationRate(float rate);
    
    // Get target synapse density
    float getTargetDensity() const;
    void setTargetDensity(float density);
    
    // Get developmental stage influence factor
    float getDevelopmentalInfluence() const;
    void setDevelopmentalInfluence(float factor);
    
    // Get homeostatic target for synapse density
    float getHomeostaticTarget() const;
    void setHomeostaticTarget(float target);
    
    // Get current synapse density
    float getCurrentDensity(class Brain* brain) const;
    
    // Check if network needs synaptogenesis
    bool needsSynaptogenesis(class Brain* brain) const;
    
    // Update synapse formation with real dynamics
    void update(class Brain* brain, class RandomGenerator& rng);
    
    // Create synapse with activity-dependent weight
    SynapseId createActivityDependentSynapse(class Brain* brain, 
                                             NeuronId source, NeuronId destination,
                                             float sourceActivity, float targetActivity,
                                             class RandomGenerator& rng);
    
    // Get statistics
    size_t getTotalSynapsesCreated() const;
    size_t getSynapsesCreatedThisStep() const;
    float getFormationEfficiency() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
