// Curiosity.hpp
#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Curiosity: Exploration motivation system
// Combines novelty detection and prediction error to drive exploration

class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get current curiosity level
    float getLevel() const;
    
    // Get exploration bias for action selection
    float getExplorationBias() const;
    
    // Get plasticity factor for learning
    float getPlasticityFactor() const;
    
    // Update curiosity system state
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm