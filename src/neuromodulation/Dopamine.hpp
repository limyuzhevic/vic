#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Implements phasic dopamine bursts (prediction error) and tonic firing
// Based on biological dopamine dynamics with reward prediction error computation
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Receive reward prediction error signal
    // Positive error = dopamine burst, negative error = dip
    void receiveRewardPredictionError(float error);
    
    // Receive actual reward
    void receiveReward(float reward);
    
    // Get prediction error history
    const std::vector<float>& getPredictionErrorHistory() const { return predictionErrorHistory_; }
    
    // Get reward history
    const std::vector<float>& getRewardHistory() const { return rewardHistory_; }
    
    // Get recent prediction error (for curiosity)
    float getRecentPredictionError() const { return recentPredictionError_; }
    
    // Reset history
    void clearHistory() { predictionErrorHistory_.clear(); rewardHistory_.clear(); }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // History for tracking prediction errors and rewards
    std::vector<float> predictionErrorHistory_;
    std::vector<float> rewardHistory_;
    float recentPredictionError_;
};

} // namespace nlm
