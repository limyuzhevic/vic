#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2 will implement real serotonin signaling

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current serotonin level
    float getLevel() const;
    void setLevel(float level);
    
    // Update serotonin dynamics
    void update(TimestepDuration dt);
    
    // Mood regulation
    void enhanceMood(float positivity);
    void reduceImpulsivity(float control);
    void modulateSocialBehavior(float sociability);
    
    // Get current state
    float getMoodLevel() const;
    float getImpulsivityLevel() const;
    float getSocialBehaviorLevel() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm