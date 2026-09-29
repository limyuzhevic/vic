#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
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
    
    // Get prediction error signal
    float getPredictionError() const;
    
    void setLearningRate(float rate);
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm