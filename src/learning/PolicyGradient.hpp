#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <random>

namespace nlm {

// PolicyGradient: Reinforcement learning with policy gradient methods
// Implements policy-based reinforcement learning where the brain learns
// policies through direct feedback from rewards
// 
// This mechanism enables:
// - Policy learning from rewards
// - Exploration-exploitation balance
// - Credit assignment for delayed rewards
// - Continuous policy optimization
// - Value-based and policy-based learning
//
// The policy gradient framework consists of:
// 1. Policy representation: Probability distribution over actions
// 2. Value estimation: Expected future rewards
// 3. Policy gradient: Update policies based on rewards
// 4. Exploration strategies: Balance exploration and exploitation
// 5. Credit assignment: Assign credit to states/actions for delayed rewards

class PolicyGradient {
public:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PolicyGradient();
    ~PolicyGradient();
    
    // Initialize with brain reference and policy parameters
    void initialize(class Brain* brain, size_t actionSpaceSize, 
                    float learningRate = 0.01f, float discountFactor = 0.95f,
                    float explorationRate = 0.1f);
    
    // Select action based on current policy
    size_t selectAction(const std::vector<float>& state, float temperature = 1.0f);
    
    // Update policy based on reward and state
    void updatePolicy(const std::vector<float>& state, size_t action,
                      float reward, const std::vector<float>& nextState);
    
    // Get action probability distribution for a state
    std::vector<float> getActionProbabilities(const std::vector<float>& state) const;
    
    // Get value estimate for a state
    float getValueEstimate(const std::vector<float>& state) const;
    
    // Get policy gradient statistics
    float getPolicyGradientMagnitude() const;
    float getExplorationRate() const;
    
    // Get experience replay memory
    const std::vector<std::tuple<std::vector<float>, size_t, float, std::vector<float>>>&
        getExperienceMemory() const;
    
    // Add experience to replay memory
    void addExperience(const std::vector<float>& state, size_t action,
                       float reward, const std::vector<float>& nextState);
    
    // Clear experience memory
    void clearExperienceMemory();
    
    // Enable/disable policy learning
    void enable(bool enable);
    bool isEnabled() const;
    
    // Configure policy parameters
    void setLearningRate(float rate);
    void setDiscountFactor(float factor);
    void setExplorationRate(float rate);
    void setTemperature(float temp);
    
    // Exploration strategies
    void setExplorationStrategy(const std::string& strategy);
    void setEpsilonGreedy(float epsilon);
    void setBoltzmannExploration(float temp);
    void setOUExploration(float mu, float sigma);
    
    // Get policy statistics
    float getEntropy() const;
    size_t getActionCount(const std::vector<float>& state) const;
    float getMaxValue() const;
    float getAverageValue() const;
    
    // Training and optimization
    void trainExperienceReplay(size_t batchSize = 32);
    void updatePolicyGradient(const std::vector<std::tuple<std::vector<float>, size_t, float, std::vector<float>>>& batch);
    
    // Policy evaluation
    void evaluatePolicy(const std::vector<std::tuple<std::vector<float>, size_t, float, std::vector<float>>>& testExperiences);
    float getPolicyPerformance() const;
    
private:
    PolicyGradient(const PolicyGradient&) = delete;
    PolicyGradient& operator=(const PolicyGradient&) = delete;
};

} // namespace nlm