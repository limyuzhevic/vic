#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Synapse pruning: removal of weak or unused synapses
// Enhanced implementation with activity-dependent elimination, developmental regulation,
// and homeostatic balance for Phase 2 structural plasticity

class Pruning {
public:
    Pruning();
    ~Pruning();
    
    // Get pruning threshold
    float getThreshold() const;
    void setThreshold(float threshold);
    
    // Get pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Get developmental influence factor
    float getDevelopmentalInfluence() const;
    void setDevelopmentalInfluence(float factor);
    
    // Get homeostatic target for synapse density
    float getHomeostaticTarget() const;
    void setHomeostaticTarget(float target);
    
    // Get synaptic strength threshold for pruning
    float getStrengthThreshold() const;
    void setStrengthThreshold(float threshold);
    
    // Get activity threshold for pruning
    float getActivityThreshold() const;
    void setActivityThreshold(float threshold);
    
    // Get current synapse density
    float getCurrentDensity(class Brain* brain) const;
    
    // Check if network needs pruning
    bool needsPruning(class Brain* brain) const;
    
    // Update pruning with real dynamics
    void update(class Brain* brain, class RandomGenerator& rng);
    
    // Force pruning of specific synapse
    bool pruneSynapse(class Synapse* synapse);
    
    // Remove weak synapse based on activity and weight
    bool removeWeakSynapse(class Brain* brain, class Synapse* synapse, 
                          float sourceActivity, float targetActivity,
                          class RandomGenerator& rng);
    
    // Get statistics
    size_t getTotalSynapsesPruned() const;
    size_t getSynapsesPrunedThisStep() const;
    float getPruningEfficiency() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
