#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// PHASE 6 IMPLEMENTATION - Real dopamine dynamics

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get/Set configuration parameters
    void setBaseline(float baseline);
    float getBaseline() const;
    
    void setPeak(float peak);
    float getPeak() const;
    
    void setDecayRate(float rate);
    float getDecayRate() const;
    
    void setReleaseRate(float rate);
    float getReleaseRate() const;
    
    // Integration with prediction system
    void updateWithPrediction(float prediction, TimestepDuration dt);
    
    // Reset system
    void reset();
    
    // Get curiosity-driven exploration drive
    float getExplorationDrive() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm