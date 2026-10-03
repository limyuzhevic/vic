// Serotonin.hpp - Neuromodulator for mood and behavioral regulation

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin modulates mood, behavioral state, and social behavior
// Reduces anxiety and promotes calm, focused behavior
// Important for motivation and emotional regulation

class Brain;

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get serotonin level
    float getLevel() const;
    
    // Get mood enhancement factor
    float getMoodFactor() const;
    
    // Update serotonin based on social and environmental factors
    void update(TimestepDuration dt, float socialCohesion, float safetyLevel);
    
    // Signal social interaction or reward
    void signalSocialReward(float socialValue, float safety);
    
    // Modulate anxiety and stress response
    void reduceAnxiety(float anxietyLevel);
    
    // Promote calm behavior and focus
    void promoteCalmFocus(class NeuralPopulation* prefrontalPopulation);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm