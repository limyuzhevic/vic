// Dopamine: Reward and reinforcement learning signal
// Version: Phase 2 Implementation
// Purpose: Implements dopamine neuromodulation for reward prediction and reinforcement learning

#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Represents the brain's reward system, signaling reward prediction errors
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level (0.0 to 1.0)
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    // Higher dopamine increases plasticity for rewarded actions
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Reward signaling methods
    // Signal delivery of expected reward
    void signalReward(float reward);
    
    // Signal reward prediction error (actual - expected reward)
    void signalRewardPredictionError(float error);
    
    // Set dopamine parameters
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayRate(float decayRate);
    void setReleaseRate(float releaseRate);
    
    // Get dopamine parameters
    float getBaseline() const;
    float getPeak() const;
    float getDecayRate() const;
    float getReleaseRate() const;
    
    // Reset dopamine state
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
