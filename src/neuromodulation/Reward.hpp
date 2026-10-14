#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

/**
 * @class Reward
 * @brief Neuromodulator representing reward and reinforcement signals
 * 
 * The Reward neuromodulator tracks accumulated rewards and provides prediction
 * error signals for reinforcement learning. It modulates plasticity based on
 * received rewards and the difference between expected and actual outcomes.
 * 
 * @details Maintains both instantaneous and accumulated reward values, with
 * history tracking for reward prediction error computation.
 */
class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    // TODO PHASE 2: Implement real reward computation from observation
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
