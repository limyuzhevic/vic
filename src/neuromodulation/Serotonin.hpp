#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior regulation
// Real implementation with serotonergic modulation effects

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
    // 5-HT modulates both approach and avoidance behaviors
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Response to social feedback
    void respondToSocialFeedback(float socialValue, float predictionError);
    
    // Modulate decision-making and impulsivity
    float modulateDecisionThreshold(float baselineThreshold, float urgency) const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
