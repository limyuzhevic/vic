#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Complete Dopamine neuromodulator implementation with real neuroscience
// and biologically plausible dynamics

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Basic neuromodulator interface
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Dopamine-specific functionality
    
    // Reward signaling with temporal processing
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Spike timing-dependent modulation
    void recordPreSpike(Timestamp time);
    void recordPostSpike(Timestamp time);
    
    // Learning signal components
    float getPredictionError() const;
    float getValuePrediction() const;
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    
    // Configuration parameters
    void setBaseline(float baseline);
    float getBaseline() const;
    
    void setPeak(float peak);
    float getPeak() const;
    
    void setDecayRate(float rate);
    float getDecayRate() const;
    
    void setReleaseRate(float rate);
    float getReleaseRate() const;
    
    void setReuptakeRate(float rate);
    float getReuptakeRate() const;
    
    // State management
    void reset();
    void snapshot();
    void restore();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm