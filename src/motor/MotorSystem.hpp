#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>

namespace nlm {

// Motor system: converts neural activity to actions
// PHASE 2: Real action selection with neural activity patterns

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity using policy gradient
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Get policy network weights (for reinforcement learning)
    const std::unordered_map<ActionType, float>& getPolicyWeights() const;
    
    // Update policy weights based on reward signals
    void updatePolicy(const Action* action, float reward, float learningRate = 0.01f);
    
    // Action selection strategies
    enum class Strategy {
        Greedy,          // Always select best action
        EpsilonGreedy,   // Greedy with epsilon probability of random exploration
        Boltzmann,       // Softmax over action preferences
        ThompsonSampling // Bayesian uncertainty sampling
    };
    
    void setStrategy(Strategy strategy);
    Strategy getStrategy() const;
    
    // Exploration parameters
    void setExplorationRate(float epsilon);
    float getExplorationRate() const;
    
    // Temperature for Boltzmann exploration
    void setTemperature(float temp);
    float getTemperature() const;
    
    // Reward prediction error for learning
    void updateRewardPrediction(const Action* action, float actualReward);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
