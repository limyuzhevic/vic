#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

/**
 * @class Dopamine
 * @brief Reward and reinforcement learning signal
 * 
 * Dopamine is a key neuromodulator in reinforcement learning, signaling
 * reward prediction errors and promoting plasticity at rewarded synapses.
 * It implements reward-based learning and motivation.
 * 
 * @details Uses PImpl idiom for forward declaration to avoid circular
 * dependencies with Brain class. Provides reward signaling and prediction
 * error processing capabilities.
 */
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Get neuromodulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Get plasticity modulation factor
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
