// Dopamine.hpp
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get dopamine level
    float getLevel() const;
    
    // Get plasticity factor for dopamine
    float getPlasticityFactor() const;
    
    // Update dopamine over time
    void update(TimestepDuration dt);
    
    // Reset dopamine state
    void reset();
    
    // Apply neuromodulatory effects
    void applyEffects();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm