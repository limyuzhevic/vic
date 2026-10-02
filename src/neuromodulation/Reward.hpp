#pragma once

#include "Environment.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Enhanced Phase 6 implementation with multi-component architecture

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
    // Enhanced multi-component reward computation
    float computeReward(const Observation& observation, std::shared_ptr<const Config> config);
    
    // Individual reward component computations
    static float computeSurvivalReward(const Observation& observation, std::shared_ptr<const Config> config);
    static float computeEfficiencyReward(const Observation& observation, std::shared_ptr<const Config> config);
    static float computeLearningReward(const Observation& observation, std::shared_ptr<const Config> config);
    static float computeExplorationReward(const Observation& observation, std::shared_ptr<const Config> config);
    
    // Reward statistics
    float getAverageReward() const;
    float getMaxReward() const;
    float getMinReward() const;
    float getAccumulatedReward() const;
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Reward modulation
    void setWeight(float weight);
    float getWeight() const;
    
    // Adaptive reward parameters
    void setSurvivalWeight(float weight);
    void setEfficiencyWeight(float weight);
    void setLearningWeight(float weight);
    void setExplorationWeight(float weight);
    
    float getSurvivalWeight() const;
    float getEfficiencyWeight() const;
    float getLearningWeight() const;
    float getExplorationWeight() const;
    
    // Reward history management
    void addToHistory(float reward);
    size_t getHistorySize() const;
    bool isHistoryFull() const;
    void setHistoryCapacity(size_t capacity);
    
    // Reward normalization
    static float normalizeReward(float reward, float min = 0.0f, float max = 1.0f);
    static float denormalizeReward(float normalizedReward, float min = 0.0f, float max = 1.0f);
    
    // Reward debugging and analysis
    std::string getRewardAnalysis() const;
    std::vector<float> getNormalizedHistory() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Component computation helpers
    static float computeSurvivalComponent(const Observation& observation, std::shared_ptr<const Config> config);
    static float computeEfficiencyComponent(const Observation& observation, std::shared_ptr<const Config> config);
    static float computeLearningComponent(const Observation& observation, std::shared_ptr<const Config> config);
    static float computeExplorationComponent(const Observation& observation, std::shared_ptr<const Config> config);
    
    // Reward combination strategies
    static float weightedSum(const std::vector<float>& components, const std::vector<float>& weights);
    static float maxReward(const std::vector<float>& components);
    static float minReward(const std::vector<float>& components);
};

} // namespace nlm