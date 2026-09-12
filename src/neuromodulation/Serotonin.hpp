#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood, social behavior, impulsivity, reward processing

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Signal social feedback
    void signalSocialFeedback(float socialReward, float isolation);
    
    // Set mood level
    void setMood(float mood);
    
    // Get mood level
    float getMood() const;
    
    // Set impulsivity level
    void setImpulsivity(float impulsivity);
    
    // Get impulsivity level
    float getImpulsivity() const;
    
    // Modulate decision making
    void modulateDecisionMaking(float decisionValue);
    
    // Enhance social bonding
    void enhanceSocialBonding();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
