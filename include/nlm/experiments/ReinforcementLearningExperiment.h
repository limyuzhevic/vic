#pragma once

#include "AdvancedExperiment.h"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <unordered_map>
#include <cmath>

namespace nlm {

/**
 * Reinforcement Learning Experiment
 * 
 * Advanced experiment that tests reinforcement learning and reward prediction
 * with Q-learning, policy gradient, and actor-critic methods.
 * Demonstrates how neural networks can learn optimal policies through
 * trial-and-error and reward-based learning.
 */
class ReinforcementLearningExperiment : public AdvancedExperiment {
public:
    // Reinforcement learning configuration
    struct ReinforcementConfig {
        // RL algorithm parameters
        float learningRate;
        float discountFactor;
        float explorationRate;
        float temperature;
        
        // Reward shaping parameters
        float rewardScale;
        float intrinsicReward;
        float extrinsicReward;
        
        // Value function parameters
        float valueFunctionWeight;
        float policyGradientWeight;
        float actorCriticWeight;
        
        // Experiment parameters
        size_t numTrials;
        float timeLimit;
        bool trackQValues;
        bool trackPolicyGradients;
        bool trackValueFunctions;
        
        ReinforcementConfig()
            : learningRate(0.01f), discountFactor(0.99f), explorationRate(0.1f),
              temperature(1.0f), rewardScale(1.0f), intrinsicReward(0.1f),
              extrinsicReward(0.9f), valueFunctionWeight(0.5f),
              policyGradientWeight(0.3f), actorCriticWeight(0.2f),
              numTrials(100), timeLimit(300.0f), trackQValues(true),
              trackPolicyGradients(true), trackValueFunctions(true) {}
    };
    
    // Q-value table
    struct QTableEntry {
        std::string state;
        std::string action;
        float value;
        float visitationCount;
        
        QTableEntry() : value(0.0f), visitationCount(0) {}
    };
    
    // Policy gradient result
    struct PolicyGradientResult {
        std::string action;
        float probability;
        float advantage;
        float gradient;
        
        PolicyGradientResult() : probability(0.0f), advantage(0.0f), gradient(0.0f) {}
    };
    
    // Value function result
    struct ValueFunctionResult {
        std::string state;
        float estimatedValue;
        float targetValue;
        float temporalDifferenceError;
        
        ValueFunctionResult() : estimatedValue(0.0f), targetValue(0.0f),
                               temporalDifferenceError(0.0f) {}
    };
    
    ReinforcementLearningExperiment();
    ~ReinforcementLearningExperiment() = default;
    
    // Experiment lifecycle implementation
    bool initialize() override;
    void setup() override;
    void run() override;
    void analyzeResults() override;
    void cleanup() override;
    
    // Configuration management
    void setReinforcementConfig(const ReinforcementConfig& config);
    const ReinforcementConfig& getReinforcementConfig() const;
    
    // Experiment parameters
    void setNumNeurons(size_t neurons) override;
    void setNumSynapses(size_t synapses) override;
    void setNumTrials(size_t trials) override;
    void setTimeLimit(double limit) override;
    
    // Reinforcement learning specific methods
    void runReinforcementLearningTrial(size_t trialNumber);
    void updateQTable(const std::string& state, const std::string& action, float reward, const std::string& nextState);
    PolicyGradientResult computePolicyGradient(const std::string& state, const std::string& action);
    ValueFunctionResult computeValueFunction(const std::string& state, float targetValue);
    std::string selectAction(const std::string& state);
    float calculateReward(float extrinsicReward, float intrinsicReward);
    
    // Results management
    const std::vector<QTableEntry>& getQTable() const;
    const std::vector<PolicyGradientResult>& getPolicyGradients() const;
    const std::vector<ValueFunctionResult>& getValueFunctions() const;
    double calculateQValueConvergence() const;
    double calculatePolicyGradientEfficiency() const;
    double calculateValueFunctionAccuracy() const;
    
    // Brain and environment setup
    std::shared_ptr<Brain> createBrain() override;
    std::shared_ptr<Environment> createEnvironment() override;
    
    // Experiment execution
    bool runExperiment(std::shared_ptr<Brain> brain, 
                       std::shared_ptr<Environment> environment) override;
    
    // Results saving/loading
    bool saveResults(const std::string& filepath) override;
    bool loadResults(const std::string& filepath) override;
    
    // Progress reporting
    double getProgress() const override;
    std::string getStatus() const override;
    
    // Reinforcement learning specific getters
    float getAverageQValue() const;
    float getPolicyGradientNorm() const;
    float getValueFunctionMSE() const;
    
private:
    // Configuration
    ReinforcementConfig config;
    
    // State tracking
    std::vector<QTableEntry> qTable;
    std::vector<PolicyGradientResult> policyGradients;
    std::vector<ValueFunctionResult> valueFunctions;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Environment> currentEnvironment;
    
    // RL state
    std::string currentState;
    std::string currentAction;
    float currentQValue;
    float currentValue;
    float episodeReward;
    
    // Random number generation
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> rewardDist;
    std::uniform_real_distribution<double> qValueDist;
    std::uniform_real_distribution<double> policyDist;
};

} // namespace nlm