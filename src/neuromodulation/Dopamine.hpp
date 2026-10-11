#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Real implementation with dynamic level control

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Configuration
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    
    // Get parameters
    float getBaseline() const;
    float getPeak() const;
    float getDecayRate() const;
    float getReleaseRate() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm