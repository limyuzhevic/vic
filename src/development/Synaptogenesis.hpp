#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Synaptogenesis: formation of new synapses
// REALIZED with activity-dependent synapse formation using Hebbian rules

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
    
    // Update synapse formation
    void update(class Brain* brain, class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
