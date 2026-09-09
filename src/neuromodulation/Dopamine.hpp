#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Real implementation with reward prediction error signaling
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
    
    // Get prediction error
    float getPredictionError() const;
    void resetPredictionError();
    
    // History and configuration
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    void setBaseline(float baseline);
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
