#pragma once

#include "Neuromodulator.hpp"
#include "../environment/Observation.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Implements sophisticated reward computation based on sensory input
// and agent-environment interactions

class Reward {
public:
    // Reward types for different sources
    enum class RewardType {
        ResourceFound = 1,           // Positive reward for finding/collecting resources
        HazardAvoided = 2,           // Positive reward for avoiding hazards
        HazardDamage = 4,            // Negative reward for taking hazard damage
        Novelty = 8,                 // Reward for novel experiences (curiosity)
        Exploration = 16,            // Reward for exploration behavior
        Performance = 32,            // Performance-based reward
        Development = 64,            // Reward for developmental progress
        Social = 128,                // Social learning rewards
        EnergyEfficiency = 256,      // Reward for efficient energy use
        Survival = 512,              // Basic survival reward
        Structural = 1024            // Structural plasticity rewards
    };
    
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta, RewardType type = RewardType::Survival);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    // Combines multiple reward types based on observation and agent state
    float computeReward(const Observation& observation,
                       float dopamineLevel = 0.0f,
                       float curiosityLevel = 0.0f,
                       float noveltyLevel = 0.0f,
                       float health = 1.0f,
                       float energy = 1.0f,
                       bool isMoving = false,
                       bool hasInteracted = false,
                       float visionCohesion = 0.0f,
                       float forwardProgress = 0.0f,
                       float explorationRate = 0.0f);
    
    // Specialized reward computation methods
    float computeResourceReward(const Observation& observation) const;
    float computeHazardReward(const Observation& observation) const;
    float computeNoveltyReward(const Observation& observation) const;
    float computeDevelopmentReward(float developmentalAge) const;
    float computeEfficiencyReward(float energyUsage, float time) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get reward statistics
    float getAverageReward() const;
    float getMaxReward() const;
    float getMinReward() const;
    size_t getHistorySize() const;
    
    // Configure reward parameters
    void setResourceValue(float value);
    void setHazardPenalty(float penalty);
    void setNoveltyMultiplier(float multiplier);
    void setExplorationFactor(float factor);
    void setSurvivalThreshold(float threshold);
    void setEfficiencyWeight(float weight);
    void setDevelopmentalRewardRate(float rate);
    
    // Get current reward parameters
    float getResourceValue() const;
    float getHazardPenalty() const;
    float getNoveltyMultiplier() const;
    float getExplorationFactor() const;
    float getSurvivalThreshold() const;
    float getEfficiencyWeight() const;
    float getDevelopmentalRewardRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
