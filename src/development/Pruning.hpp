#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Synapse pruning: removal of weak or unused synapses
// PLACEHOLDER - Phase 2 will implement real pruning

class Pruning {
public:
    Pruning(nullptr);
    ~Pruning(nullptr);
    
    // Get pruning threshold
    float getThreshold(nullptr) const;
    void setThreshold(float threshold);
    
    // Get pruning rate
    float getPruningRate(nullptr) const;
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
