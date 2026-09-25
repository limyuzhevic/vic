#pragma once

#include "Neuromodulator.hpp"
#include <vector>
#include <string>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// COMPLETE - Full implementation with prediction error, working memory, reward processing
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalExtinction(float reward);
    void signalOmission(float reward);
    
    // Working memory updating
    void updateWorkingMemory(float predictionError);
    
    // Reward prediction
    void updateRewardPrediction(const std::vector<float>& predictedRewards, 
                               const std::vector<float>& actualRewards);
    
    // Reward history tracking
    const std::vector<float>& getRewardHistory() const { return pImpl->rewardHistory; }
    void clearRewardHistory() { pImpl->rewardHistory.clear(); }
    
    // Learning rate modulation
    float getLearningRate() const { return pImpl->learningRate; }
    void setLearningRate(float rate) { pImpl->learningRate = rate; }
    
    // Credit assignment
    void assignCredit(float predictionError, float eligibilityTrace);
    
    // Get current prediction error
    float getPredictionError() const { return pImpl->predictionError; }
    
    // Get expected value
    float getExpectedValue() const { return pImpl->expectedValue; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm