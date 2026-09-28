#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement signal
// Mediates reward prediction error and reinforcement learning

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get dopamine level
    float getLevel() const;
    
    // Update dopamine based on reward prediction error
    void update(TimestepDuration dt);
    
    // Get plasticity factor (modulates synaptic learning)
    float getPlasticityFactor() const;
    
    // Set dopamine level (from reward prediction error)
    void setLevel(float level);
    
    // Reset dopamine state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm