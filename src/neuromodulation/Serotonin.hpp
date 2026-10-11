#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood regulation and social behavior modulation
// Real-world based: 5-HT regulates mood, impulsivity, social behavior, and decision making
// Clinical relevance: SSRIs for depression, impulsive aggression, social behavior disorders

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference for integration
    void initialize(class Brain* brain);
    
    // Get current 5-HT level
    float getLevel() const;
    void setLevel(float level);
    
    // Update 5-HT dynamics based on social context and reward outcomes
    void update(float socialEngagement, float rewardOutcome, TimestepDuration dt);
    
    // Get 5-HT-modulated plasticity factor
    float getPlasticityFactor() const;
    
    // Advanced functionality
    void promoteSocialBehavior(NeuronId targetNeuron);
    void regulateImpulsivity(float inhibitionTarget);
    void modulateDecisionMaking(float deliberationTime);
    void enhanceWellbeing(float positiveFeedback);
    
    // Configuration parameters
    void setSocialSensitivity(float sensitivity);
    void setRewardWeight(float weight);
    void setInhibitionBaseline(float baseline);
    void setMoodStabilization(float stability);
    
    float getSocialSensitivity() const;
    float getRewardWeight() const;
    float getInhibitionBaseline() const;
    float getMoodStabilization() const;
    
    // Reset for new episode
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm