#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Real implementation for reward prediction error signaling

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
    
    // Prediction error computation
    void computePredictionError(float predicted, float actual);
    
    // Dopamine dynamics
    float getTemporalIntegral() const;
    void resetTemporalIntegral();
    
    // Influence on learning
    float getLTDWeight() const;
    float getLTPWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm