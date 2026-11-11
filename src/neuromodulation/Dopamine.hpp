#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Dopamine neuromodulator for reward prediction error and reward-based learning
// Implements dopamine dynamics including baseline, phasic bursts, and prediction errors

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current dopamine level
    float getLevel() const;
    
    // Get baseline dopamine level (tonic activity)
    float getBaselineLevel() const;
    
    // Set baseline level
    void setBaselineLevel(float level);
    
    // Get prediction error (reward prediction error)
    float getPredictionError() const;
    
    // Compute and update dopamine based on reward and prediction error
    void update(float reward, float predictedReward, float rewardPredictionError, TimestepDuration dt);
    
    // Get eligibility trace for reward-modulated plasticity
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Get dopamine history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Dopamine modulates plasticity based on prediction error
    // Returns modulation factor (0.0 to 1.0)
    float getPlasticityModulation() const;
    
    // Reset to initial state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm