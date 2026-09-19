#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Proper implementation with P-impl pattern

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Regulate impulsivity and behavioral flexibility
    void reduceImpulsivity(float amount);
    void enhanceBehavioralFlexibility(float amount);
    
    // Respond to rewards and predictions
    void respondToReward(float reward);
    void respondToPredictionError(float error);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm