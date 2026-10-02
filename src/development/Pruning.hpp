#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Synapse pruning: removal of weak or unused synapses
// Phase 6: Simplified pruning implementation
// Current implementation: Basic synapse removal with activity-dependent pruning
// TODO FUTURE PHASE: Implement real pruning with homeostatic mechanisms

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
    
    // Update pruning
    void update(class Brain* brain, class RandomGenerator& rng);
    
    // Force pruning of specific synapse
    bool pruneSynapse(class Synapse* synapse);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
