// Reward signal for reinforcement learning
// Phase 6: Implement real reward computation based on observation

#include "Observation.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

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
    // Phase 6: Real reward computation based on observation
    float computeReward(const Observation& observation, std::shared_ptr<const Config> config);
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Configuration-based reward components
    static float computeSurvivalReward(const Observation& observation, std::shared_ptr<const Config> config);
    static float computeEfficiencyReward(const Observation& observation, std::shared_ptr<const Config> config);
    static float computeLearningReward(const Observation& observation, std::shared_ptr<const Config> config);
    static float computeExplorationReward(const Observation& observation, std::shared_ptr<const Config> config);
    
    // Get accumulated reward (lifetime total)
    float getAccumulatedReward() const;
    
    // Get number of reward samples
    size_t getHistorySize() const;
    
    // Get average reward
    float getAverageReward() const;
    
    // Get maximum reward in history
    float getMaxReward() const;
    
    // Get minimum reward in history
    float getMinReward() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm