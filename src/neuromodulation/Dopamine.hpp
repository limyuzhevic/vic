#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Computes dopamine release in response to rewards and prediction errors
// High dopamine increases plasticity and reinforces rewarded behaviors

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
    
    // Getters and setters for additional dopamine parameters
    float getBaseline() const { return baseline_; }
    void setBaseline(float baseline) { baseline_ = baseline; }
    
    float getPeak() const { return peak_; }
    void setPeak(float peak) { peak_ = peak; }
    
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    float getReleaseRate() const { return releaseRate_; }
    void setReleaseRate(float rate) { releaseRate_ = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    float baseline_;
    float peak_;
    float decayRate_;
    float releaseRate_;
};

} // namespace nlm