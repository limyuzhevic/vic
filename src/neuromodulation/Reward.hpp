#pragma once

#include "../core/Config/Config.hpp"
#include "../world/Observation.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../environment/Observation.hpp"
#include <vector>

namespace nlm {

// Forward declarations for implementation details
namespace world {
    class Observation;
}

namespace environment {
    class Observation;
}

// Reward signal for reinforcement learning
// Phase 2 implementation: Real reward computation from environmental observations

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
    // Real-time reward computation based on sensory input, internal state, and environmental context
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Reward configuration and tracking
    void initialize(std::shared_ptr<Config> config);
    bool isInitialized() const;
    
    // Get reward statistics
    float getLastStepReward() const;
    float getEpisodeTotalReward() const;
    float getGoalProgress() const;
    float getHazardRisk() const;
    
    // Get reward components
    float getGoalReward() const;
    float getResourceReward() const;
    float getHazardPenalty() const;
    float getStepPenalty() const;
    float getExplorationReward() const;
    
    // Set reward parameters
    void setGoalReward(float reward);
    void setResourceReward(float reward);
    void setHazardPenalty(float penalty);
    void setStepPenalty(float penalty);
    void setExplorationReward(float reward);
    
    // Update internal state
    void updateTemporalState();
    void resetEpisodeState();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
