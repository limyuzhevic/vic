#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Real implementation with actual reward prediction error signaling

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling with prediction error
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get internal state
    float getBaseline() const;
    void setBaseline(float baseline);
    float getPeak() const;
    void setPeak(float peak);
    float getDecayRate() const;
    void setDecayRate(float decayRate);
    float getReleaseRate() const;
    void setReleaseRate(float releaseRate);
    
    // Get reward history for analysis
    const std::vector<float>& getRecentRewards() const;
    size_t getRewardHistorySize() const;
    
    // Reset to initial state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
