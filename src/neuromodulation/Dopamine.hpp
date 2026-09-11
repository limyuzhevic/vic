#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>

namespace nlm {

// Enhanced Dopamine implementation with RL-inspired dynamics
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
    
    // Additional methods for enhanced functionality
    void initialize(Brain* brain);
    void setBaseline(float baseline);
    void setReleaseRate(float rate);
    void setDecayRate(float rate);
    void setPredictionErrorGain(float gain);
    
    float getPredictionError() const;
    float getRewardPrediction() const;
    float getTemporalDifference() const;
    
    const std::vector<float>& getRewardHistory() const;
    const std::vector<float>& getErrorHistory() const;
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm