#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../brain/AgentBrain.hpp"
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Reward computation system with biological plausibility
// Implements comprehensive reward signaling for reinforcement learning

class Reward {
public:
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward (for backward compatibility)
    void add(float delta);
    
    // Reset accumulated reward (for backward compatibility)
    void reset();
    
    // Compute reward from observation using biologically plausible models
    // Integrates multiple reward sources with temporal discounting
    float computeReward(const Observation& observation);
    
    // Reward history with timestamps for temporal analysis
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Advanced reward computation methods
    
    // Compute reward from sensory input, position, action, and outcome
    float computeOutcomeReward(const SensoryInput& sensoryInput, 
                              const std::vector<float>& position,
                              const class Action& action,
                              float outcome) const;
    
    // Compute action-based reward (intrinsic/extrinsic motivation)
    float computeActionReward(const class Action& action) const;
    
    // Compute state-based reward from current observation
    float computeStateReward(const Observation& observation) const;
    
    // Reward statistics and analysis
    float getMeanReward() const;
    float getVarianceReward() const;
    float getMaxReward() const;
    float getMinReward() const;
    float getRewardRange() const;
    
    // Temporal reward analysis
    float getDiscountedFutureReward(float discountFactor, int steps) const;
    float computeRewardPredictionError(float predictedReward) const;
    
    // Reward learning and adaptation
    void updateRewardHistory(float reward);
    void updateRewardPrediction(float newPrediction);
    void adaptRewardSensitivity(float sensitivity);
    
    // Neuromodulation integration
    void integrateWithDopamine(Dopamine* dopamine);
    void integrateWithCuriosity(Curiosity* curiosity);
    void integrateWithBrain(AgentBrain* brain);
    void signalToNeuromodulators(float reward, float predictionError);
    
    // Advanced features
    
    // Temporal difference learning for reward prediction
    void updateTDLearning(float reward, float predictedReward, float discountFactor = 0.99f);
    float getTemporalDifference() const;
    float getEligibilityTrace() const;
    
    // Reward sensitivity modulation (dopamine interaction)
    void modulateRewardSensitivity(float sensitivity);
    float getRewardSensitivity() const;
    
    // Reward valuation based on experience
    float getExperienceBasedValue(const Observation& observation) const;
    void updateExperienceValue(const Observation& observation, float reward);
    
    // Adaptation to changing reward environments
    void detectRewardEnvironmentChange(float threshold = 0.1f);
    bool isEnvironmentChanging() const;
    void resetAdaptationState();
    
    // Curiosity and exploration integration
    void integrateWithCuriositySystem(float curiosityLevel, float novelty);
    float getExplorationBonus() const;
    
    // Reward components separation
    float getExtrinsicReward() const;
    float getIntrinsicReward() const;
    void setExtrinsicWeight(float weight);
    void setIntrinsicWeight(float weight);
    
    // Historical analysis and pattern detection
    float getHistoricalMean(int windowSize = 100) const;
    float getTrend(float windowSize = 50) const;
    bool detectRewardPattern(const std::vector<float>& pattern) const;
    
    // Time-weighted reward computation
    struct RewardEntry {
        float value;
        Timestamp timestamp;
        float discountFactor;
        
        RewardEntry(float val, Timestamp ts, float df = 0.99f) 
            : value(val), timestamp(ts), discountFactor(df) {}
    };
    
    // Get time-weighted accumulated reward
    float getTimeWeightedReward(Timestamp currentTime, float decayRate = 0.01f) const;
    
    // Working memory integration for reward storage
    void storeRewardInWorkingMemory(float reward, const std::vector<float>& context);
    std::vector<float> retrieveRelevantRewards(const std::vector<float>& context) const;
    
    // Reward prediction and error computation
    void predictNextReward(float predictedValue);
    float getPredictedReward() const;
    float getCurrentPredictionError() const;
    
    // Reward adaptation state management
    struct AdaptationState {
        float baselineReward;
        float sensitivity;
        float adaptationRate;
        Timestamp lastAdaptation;
        
        AdaptationState() : baselineReward(0.0f), sensitivity(1.0f), 
                           adaptationRate(0.01f), lastAdaptation(0.0) {}
    };
    
    AdaptationState getAdaptationState() const;
    void updateAdaptationState(float reward, float predictionError);
    
    // Configuration and initialization
    void configure(float extrinsicWeight = 0.7f, float intrinsicWeight = 0.3f,
                   float discountFactor = 0.95f, float sensitivity = 1.0f);
    void initializeRewardSystem(AgentBrain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
