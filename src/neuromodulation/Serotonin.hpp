#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin (5-HT): Mood regulation, sleep/wake cycle, reward processing, and anxiety modulation
// 
// Key functions:
// - Regulates mood and emotional states
// - Controls sleep/wake cycle
// - Modulates reward processing and reinforcement
// - Regulates anxiety and stress response
// - Controls impulsivity and decision-making
// - Influences social behavior and bonding
// - Affects sleep architecture and dreaming

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get 5-HT level
    float getLevel() const;
    void setLevel(float level);
    
    // Get neuromodulator name
    const char* getName() const;
    
    // Get neuromodulator effects on plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Signal specific serotonergic effects
    // Regulate mood state
    void signalMood(float level);
    
    // Regulate sleep/wake cycle
    void signalSleepWake(float cycle);
    
    // Enhance reward processing and learning
    void signalRewardProcessing(float level);
    
    // Modulate anxiety and stress response
    void signalAnxiety(float level);
    
    // Control impulsivity and decision-making
    void signalImpulsivity(float level);
    
    // Modulate social behavior and bonding
    void signalSocialBehavior(float level);
    
    // Get current mood level
    float getMood() const;
    
    // Get sleep/wake regulation
    float getSleepWake() const;
    
    // Get reward processing enhancement
    float getRewardProcessing() const;
    
    // Get anxiety level
    float getAnxiety() const;
    
    // Get impulsivity control
    float getImpulsivity() const;
    
    // Get social behavior modulation
    float getSocialBehavior() const;
    
    // Reset for new learning episode
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm