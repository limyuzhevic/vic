#pragma once

#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Real dopamine dynamics with temporal filtering, phasic/tonic signaling, and prediction error
class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    
    // Mode control
    void setMode(float mode);  // 0.0f = tonic only, 1.0f = phasic only
    void setPhasicGain(float gain);
    void setTonicGain(float gain);
    void setTimeConstant(float tc);
    void setLearningRate(float lr);
    
    // Get current neuromodulator levels
    float getPhasicComponent() const;
    float getTonicComponent() const;
    float getPredictionError() const;
    float getRewardPredictionError() const;
    
    // Plasticity factor computation
    float getPlasticityFactor() const;
    float getPlasticityModulation() const;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalNovelty(float novelty);
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Reward history tracking
    const std::vector<float>& getRewardHistory() const;
    void clearHistory();
    
    // Exploration value
    float getExplorationValue() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper method to update components
    void updateComponents();
};

} // namespace nlm
