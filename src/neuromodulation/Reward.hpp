#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Enhanced reward computation with exploration bonus

class Reward {
public:
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // State management
    float getAccumulatedReward() const;
    float getExplorationBonus() const;
    void setPunishmentSensitivity(float sensitivity);
    
    // Update
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm