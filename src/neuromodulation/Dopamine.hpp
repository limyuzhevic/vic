#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

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
    
    // Get expected reward (for prediction error calculation)
    float getExpectedReward() const { return expectedReward; }
    
    // Set baseline level (for developmental changes)
    void setBaseline(float baseline) { pImpl->baseline = baseline; }
    
    // Track recent activity for adaptive plasticity
    void recordActivity() { recentActivityCount++; }
    void resetActivityCounter() { recentActivityCount = 0; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float expectedReward;
    int recentActivityCount;
};

} // namespace nlm