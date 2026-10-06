#pragma once

#include "Neuromodulator.hpp"
#include <vector>
#include <random>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Real-time serotonin dynamics for mood regulation, 
// behavioral flexibility, and social interaction modulation

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Mood-related functions
    void signalReward(float reward, bool predictionError);
    void increaseMood(float delta);
    
    // Behavioral flexibility functions
    void introduceBehavioralVariability(float strength);
    
    // Social behavior functions
    void updateSocialBehavior(float interactionQuality);
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
