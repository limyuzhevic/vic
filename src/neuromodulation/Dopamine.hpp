#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Phase 2 implementation - real dopamine dynamics for reward prediction error
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
    
    // Reward learning
    void learnFromOutcome(float reward, float predictedReward, TimestepDuration dt);
    
    // Dopamine state management
    void reset();
    float getBaseline() const { return baseline_; }
    void setBaseline(float baseline) { baseline_ = baseline; }
    float getPeakLevel() const { return peak_; }
    float getReleaseRate() const { return releaseRate_; }
    void setReleaseRate(float rate) { releaseRate_ = rate; }
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    float baseline_;
    float peak_;
    float releaseRate_;
    float decayRate_;
};

} // namespace nlm
