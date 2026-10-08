#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// REAL IMPLEMENTATION - Serotonin regulates mood, social behavior, and impulsivity
// Biological roles:
// - Positive mood: Enhances attention to rewards, promotes exploration
// - Negative mood: Increases persistence on difficult tasks, promotes social withdrawal
// - High serotonin: Can lead to impulsivity and aggression in some contexts
// - Low serotonin: Associated with depression, anxiety, and social withdrawal

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Get modulator name
    const char* getName() const { return "5-HT"; }
    
    // Get current concentration/level
    float getLevel() const { return pImpl->currentLevel; }
    void setLevel(float level) { pImpl->currentLevel = level; }
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    void setPlasticityFactor(float factor) { pImpl->plasticityFactor = factor; }
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Serotonin signaling
    void signalMoodChange(float moodDelta);
    void signalSocialExperience(float socialReward);
    void signalRewardPrediction(float predictedReward, float actualReward);
    
    // State management
    void reset() {
        pImpl->currentLevel = 0.5f;
        pImpl->accumulatedSocialLearning = 0.0f;
        pImpl->impulsivity = 0.5f;
        pImpl->mood = 0.5f;
        pImpl->plasticityFactor = 1.0f;
    }
    
    float getMood() const { return pImpl->mood; }
    float getImpulsivity() const { return pImpl->impulsivity; }
    float getSocialLearningAccumulation() const { return pImpl->accumulatedSocialLearning; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
