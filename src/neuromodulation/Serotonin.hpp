#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Serotonergic system implementation
// Integrates mood, impulsivity, and social behavior

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const { return "5-HT"; }
    float getLevel() const { return level_; }
    void setLevel(float level);
    
    float getPlasticityFactor() const;
    void update(float dt);
    
    // Integration with other systems
    void integrateDopamine(float daLevel);
    void integrateAcetylcholine(float achLevel);
    void integrateNorepinephrine(float neLevel);
    
    // Mood regulation
    float getMoodLevel() const;
    void setMoodLevel(float mood);
    
    // Impulsivity control
    float getImpulsivity() const;
    void updateImpulsivity(float dt);
    void reduceImpulsivity(float amount);
    
    // Social behavior
    float getSocialMotivation() const;
    void updateSocialMotivation(float dt);
    
    // State management
    void reset();
    
private:
    float level_;
    float baseline_;
    float peak_;
    float decayRate_;
    float releaseRate_;
    
    // Mood
    float moodLevel_;
    float moodStability_;
    
    // Impulsivity
    float impulsivity_;
    float lastRewardDelay_;
    
    // Social motivation
    float socialMotivation_;
    float socialLearningRate_;
    
    // Integration weights
    float daWeight_;
    float achWeight_;
    float neWeight_;
};

} // namespace nlm
