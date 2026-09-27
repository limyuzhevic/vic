#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Serotonin (5-HT): Mood, impulsivity, and learning modulation
// Biologically inspired:
// - Raphe nuclei serotonin neurons encode reward prediction errors
// - 5-HT modulates learning rates and behavioral flexibility
// - Reduces impulsive actions, promotes more deliberate decision-making
// - Influences exploration vs exploitation balance
// - Affects social behavior and status assessment

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get serotonin level (0-1)
    float getLevel() const;
    
    // Set level with clamping
    void setLevel(float level);
    
    // Get learning enhancement factor (0-2)
    float getLearningEnhancement() const;
    
    // Get decision stability factor (0-1.5)
    float getDecisionStability() const;
    
    // Update serotonin dynamics
    void update(float predictionError, float reward, TimestepDuration dt);
    
    // Signal reward prediction error
    void signalPredictionError(float error);
    
    // Signal reward feedback
    void signalReward(float reward);
    
    // Signal social status update
    void signalSocialStatus(float statusChange);
    
    // Get learning modulation strength for synapse
    float getLearningModulation(class Synapse* synapse) const;
    
    // Get behavioral flexibility factor
    float getBehavioralFlexibility() const;
    
    // Get impulsivity reduction factor
    float getImpulsivityReduction() const;
    
    // Get reward valuation modulation
    float getRewardValuation(class SimulationStep step) const;
    
    // Reset serotonin state
    void reset();
    
    // Get current learning rate modifier
    float getLearningRateModifier() const { return learningRateModifier_; }
    
    // Get behavioral exploration drive
    float getExplorationDrive() const { return explorationDrive_; }
    
    // Get status-seeking motivation
    float getStatusMotivation() const { return statusMotivation_; }

private:
    struct Impl;
    Impl* pImpl;
    
    Brain* brain_;
    
    // Current serotonin state
    float currentLevel_;
    float learningRateModifier_;
    float decisionStability_;
    float explorationDrive_;
    float statusMotivation_;
    
    // Prediction history
    std::vector<float> predictionErrorHistory_;
    std::vector<float> rewardHistory_;
    
    // Social status tracking
    float currentStatus_;
    float statusTrend_;
    
    // Behavioral state
    float impulsivityLevel_;
    float deliberationTime_;
    
    // Physiological parameters
    float predictionErrorSensitivity_;
    float rewardSensitivity_;
    float statusSensitivity_;
    
    // Adaptive mechanisms
    float adaptationRate_;
    float homeostaticRate_;
    
    // History for temporal integration
    std::vector<std::pair<SimulationStep, float>> rewardHistoryWithTime_;
    std::vector<std::pair<SimulationStep, float>> predictionErrorHistoryWithTime_;
    
    // Memory traces
    std::vector<float> pastRewards_;
    std::vector<float> pastPredictionErrors_;
};

} // namespace nlm