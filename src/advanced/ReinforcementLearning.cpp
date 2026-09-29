#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../environment/Environment.hpp"

namespace nlm {

// Reinforcement learning state
struct RLState {
    std::string id;
    std::vector<double> observations;
    std::vector<double> actions;
    std::vector<double> rewards;
    double value;
    double policy;
    std::map<std::string, double> features;
    double entropy;
    
    RLState(const std::string& stateId, const std::vector<double>& obs,
            const std::vector<double>& act, const std::vector<double>& rew)
        : id(stateId), observations(obs), actions(act), rewards(rew),
          value(0.0), policy(0.0), entropy(0.0) {}
};

// Action
struct RLActions {
    std::string id;
    std::vector<double> actionVector;
    double probability;
    double value;
    double logProbability;
    
    RLActions(const std::string& actionId, const std::vector<double>& actionVec)
        : id(actionId), actionVector(actionVec), probability(0.0), value(0.0),
          logProbability(0.0) {}
};

// RL environment
class RLEnvironment {
private:
    std::string envId;
    std::vector<double> stateSpace;
    std::vector<double> actionSpace;
    double gamma;  // Discount factor
    double epsilon;  // Exploration rate
    std::mt19937 rng;
    
public:
    RLEnvironment(const std::string& id, const std::vector<double>& stateDim,
                 const std::vector<double>& actionDim, double gamma = 0.99,
                 double epsilon = 0.1)
        : envId(id), stateSpace(stateDim), actionSpace(actionDim),
          gamma(gamma), epsilon(epsilon), rng(std::random_device{}()) {}
    
    // Take action and get next state
    std::pair<RLState, double> step(const RLState& currentState, const RLActions& action) {
        // Simulate environment transition
        std::vector<double> nextObservations = currentState.observations;
        std::vector<double> nextActions = action.actionVector;
        
        // Add some noise for stochasticity
        std::normal_distribution<double> noise(0.0, 0.1);
        for (auto& obs : nextObservations) {
            obs += noise(rng);
        }
        
        // Generate reward based on action and state
        double reward = calculateReward(currentState, action);
        
        std::string nextStateId = "state_" + std::to_string(nextObservations.size());
        RLState nextState(nextStateId, nextObservations, nextActions, {reward});
        
        return {nextState, reward};
    }
    
    // Reset environment
    RLState reset() {
        std::vector<double> initialObservations = {
            0.0, 0.0, 0.0, 0.0, 0.0  // Initial state
        };
        std::vector<double> initialActions = {0.0, 0.0};
        std::vector<double> initialRewards = {0.0};
        
        std::string initialStateId = "state_initial";
        return RLState(initialStateId, initialObservations, initialActions, initialRewards);
    }
    
    // Calculate reward based on state and action
    double calculateReward(const RLState& state, const RLActions& action) {
        double reward = 0.0;
        
        // Reward based on action magnitude
        for (double actionVal : action.actionVector) {
            reward += std::abs(actionVal);
        }
        
        // Reward based on state observations
        for (double obs : state.observations) {
            reward += obs * 0.1;
        }
        
        // Small penalty for large actions (control regularization)
        double actionMagnitude = 0.0;
        for (double actionVal : action.actionVector) {
            actionMagnitude += actionVal * actionVal;
        }
        reward -= actionMagnitude * 0.01;
        
        return reward;
    }
    
    // Get all possible actions
    std::vector<RLActions> getAllActions() const {
        std::vector<RLActions> actions;
        
        // Generate discrete action set
        for (int i = 0; i < 5; ++i) {  // 5 discrete actions
            std::vector<double> actionVec = {
                (i - 2) * 0.5,  // Action values
                (i % 3) - 1
            };
            actions.emplace_back("action_" + std::to_string(i), actionVec);
        }
        
        // Add epsilon-greedy exploration
        for (auto& action : actions) {
            if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) < epsilon) {
                action.probability = 0.1;  // Explore
            } else {
                action.probability = 0.9;  // Exploit
            }
        }
        
        return actions;
    }
};

// Policy network
class PolicyNetwork {
private:
    std::vector<size_t> layerSizes;
    std::vector<std::vector<double>> weights;
    std::vector<double> biases;
    std::string activationFunction;
    double learningRate;
    std::mt19937 rng;
    
public:
    PolicyNetwork(const std::vector<size_t>& layers, const std::string& activation = "relu",
                  double lr = 0.001)
        : layerSizes(layers), activationFunction(activation), learningRate(lr),
          rng(std::random_device{}()) {
        
        // Initialize weights with Xavier initialization
        for (size_t i = 0; i < layerSizes.size() - 1; ++i) {
            size_t inputSize = layerSizes[i];
            size_t outputSize = layerSizes[i + 1];
            
            std::vector<double> layerWeights(inputSize * outputSize, 0.0);
            for (auto& w : layerWeights) {
                w = std::normal_distribution<double>(0.0, std::sqrt(2.0 / (inputSize + outputSize)))(rng);
            }
            
            weights.push_back(layerWeights);
            biases.push_back(std::vector<double>(outputSize, 0.0));
        }
    }
    
    std::vector<double> forward(const std::vector<double>& input) {
        std::vector<double> current = input;
        
        for (size_t i = 0; i < weights.size(); ++i) {
            std::vector<double> next(current.size() * layerSizes[i + 1], 0.0);
            
            for (size_t j = 0; j < current.size(); ++j) {
                for (size_t k = 0; k < layerSizes[i + 1]; ++k) {
                    double sum = biases[i][k];
                    for (size_t l = 0; l < layerSizes[i]; ++l) {
                        sum += current[j * layerSizes[i] + l] * weights[i][l * layerSizes[i + 1] + k];
                    }
                    
                    if (activationFunction == "relu") {
                        next[j * layerSizes[i + 1] + k] = std::max(0.0, sum);
                    } else if (activationFunction == "sigmoid") {
                        next[j * layerSizes[i + 1] + k] = 1.0 / (1.0 + std::exp(-sum));
                    } else {
                        next[j * layerSizes[i + 1] + k] = sum;  // Linear
                    }
                }
            }
            
            current = next;
        }
        
        return current;
    }
    
    void update(const std::vector<double>& input, const RLActions& action,
                double targetValue, double advantage) {
        std::vector<double> output = forward(input);
        
        // Calculate policy gradient
        double policyGradient = (targetValue - output[0]) * learningRate;
        
        // Update weights (simplified policy gradient)
        for (auto& layer : weights) {
            for (auto& w : layer) {
                w += policyGradient * input.back();  // Simplified update
            }
        }
        
        for (auto& b : biases) {
            for (auto& bias : b) {
                bias += policyGradient;
            }
        }
    }
};

// Value network
class ValueNetwork {
private:
    std::vector<size_t> layerSizes;
    std::vector<std::vector<double>> weights;
    std::vector<double> biases;
    std::string activationFunction;
    double learningRate;
    std::mt19937 rng;
    
public:
    ValueNetwork(const std::vector<size_t>& layers, const std::string& activation = "relu",
                 double lr = 0.001)
        : layerSizes(layers), activationFunction(activation), learningRate(lr),
          rng(std::random_device{}()) {
        
        for (size_t i = 0; i < layerSizes.size() - 1; ++i) {
            size_t inputSize = layerSizes[i];
            size_t outputSize = layerSizes[i + 1];
            
            std::vector<double> layerWeights(inputSize * outputSize, 0.0);
            for (auto& w : layerWeights) {
                w = std::normal_distribution<double>(0.0, std::sqrt(2.0 / (inputSize + outputSize)))(rng);
            }
            
            weights.push_back(layerWeights);
            biases.push_back(std::vector<double>(outputSize, 0.0));
        }
    }
    
    double forward(const std::vector<double>& input) {
        std::vector<double> current = input;
        
        for (size_t i = 0; i < weights.size(); ++i) {
            std::vector<double> next(current.size() * layerSizes[i + 1], 0.0);
            
            for (size_t j = 0; j < current.size(); ++j) {
                for (size_t k = 0; k < layerSizes[i + 1]; ++k) {
                    double sum = biases[i][k];
                    for (size_t l = 0; l < layerSizes[i]; ++l) {
                        sum += current[j * layerSizes[i] + l] * weights[i][l * layerSizes[i + 1] + k];
                    }
                    
                    if (activationFunction == "relu") {
                        next[j * layerSizes[i + 1] + k] = std::max(0.0, sum);
                    } else if (activationFunction == "sigmoid") {
                        next[j * layerSizes[i + 1] + k] = 1.0 / (1.0 + std::exp(-sum));
                    } else {
                        next[j * layerSizes[i + 1] + k] = sum;
                    }
                }
            }
            
            current = next;
        }
        
        return current[0];  // Return single value
    }
    
    void update(const std::vector<double>& input, double targetValue) {
        std::vector<double> output = forward(input);
        
        double error = targetValue - output[0];
        double gradient = error * learningRate;
        
        for (auto& layer : weights) {
            for (auto& w : layer) {
                w += gradient * input.back();
            }
        }
        
        for (auto& b : biases) {
            for (auto& bias : b) {
                bias += gradient;
            }
        }
    }
};

// Advanced reinforcement learning (A2C - Advantage Actor-Critic)
class AdvancedRL {
private:
    std::unique_ptr<RLEnvironment> environment;
    std::unique_ptr<PolicyNetwork> policyNet;
    std::unique_ptr<ValueNetwork> valueNet;
    double gamma;  // Discount factor
    double gaeLambda;  // GAE lambda
    double entropyCoeff;  // Entropy coefficient
    double valueLossCoeff;  // Value loss coefficient
    std::vector<RLState> episodeStates;
    std::vector<RLActions> episodeActions;
    std::vector<double> episodeRewards;
    std::vector<double> episodeValues;
    std::vector<double> episodeLogProbs;
    double totalReward;
    int episodeLength;
    
public:
    AdvancedRL(const Config& config) {
        // Initialize environment
        std::vector<double> stateDim = {10, 10, 5, 3};  // Example state dimensions
        std::vector<double> actionDim = {2, 2};  // Example action dimensions
        environment = std::make_unique<RLEnvironment>("main_env", stateDim, actionDim);
        
        // Initialize networks
        std::vector<size_t> policyLayers = {40, 20, 5};  // Input: state, hidden: 20, output: 5 actions
        std::vector<size_t> valueLayers = {40, 20, 1};  // Input: state, hidden: 20, output: value
        
        policyNet = std::make_unique<PolicyNetwork>(policyLayers, "relu", 0.001);
        valueNet = std::make_unique<ValueNetwork>(valueLayers, "relu", 0.001);
        
        // Configuration parameters
        gamma = config.getOr("rl.gamma", 0.99);
        gaeLambda = config.getOr("rl.gae_lambda", 0.95);
        entropyCoeff = config.getOr("rl.entropy_coeff", 0.01);
        valueLossCoeff = config.getOr("rl.value_loss_coeff", 0.5);
        
        totalReward = 0.0;
        episodeLength = 0;
    }
    
    void startEpisode() {
        episodeStates.clear();
        episodeActions.clear();
        episodeRewards.clear();
        episodeValues.clear();
        episodeLogProbs.clear();
        
        totalReward = 0.0;
        episodeLength = 0;
        
        // Reset environment
        RLState initialState = environment->reset();
        episodeStates.push_back(initialState);
    }
    
    RLActions selectAction(const RLState& state) {
        std::vector<double> stateFeatures = extractFeatures(state);
        std::vector<double> actionDist = policyNet->forward(stateFeatures);
        
        // Get all possible actions
        std::vector<RLActions> allActions = environment->getAllActions();
        
        // Select action based on policy
        std::discrete_distribution<int> actionDistrib(actionDist.begin(), actionDist.end());
        int selectedAction = actionDistrib(environment->rng);
        
        // Get action vector
        std::vector<double> selectedActionVec = allActions[selectedAction].actionVector;
        RLActions action("action_" + std::to_string(selectedAction), selectedActionVec);
        
        // Calculate log probability
        action.logProbability = std::log(actionDist[selectedAction] + 1e-8);
        
        // Store for training
        episodeActions.push_back(action);
        
        return action;
    }
    
    void takeStep(const RLState& state, const RLActions& action) {
        // Take step in environment
        auto result = environment->step(state, action);
        RLState nextState = result.first;
        double reward = result.second;
        
        // Store transition
        episodeStates.push_back(nextState);
        episodeRewards.push_back(reward);
        
        totalReward += reward;
        episodeLength++;
        
        // Store state value for GAE calculation
        double value = valueNet->forward(extractFeatures(nextState));
        episodeValues.push_back(value);
    }
    
    void finishEpisode() {
        // Calculate advantages and return
        std::vector<double> returns;
        std::vector<double> advantages;
        
        // Calculate returns
        double runningReturn = 0.0;
        for (int t = episodeRewards.size() - 1; t >= 0; --t) {
            runningReturn = episodeRewards[t] + gamma * runningReturn;
            returns.push_back(runningReturn);
        }
        std::reverse(returns.begin(), returns.end());
        
        // Calculate advantages using GAE
        double runningAdvantage = 0.0;
        for (int t = episodeRewards.size() - 1; t >= 0; --t) {
            double delta = episodeRewards[t] + gamma * episodeValues[t] - episodeValues[t-1];
            runningAdvantage = delta + gaeLambda * gamma * runningAdvantage;
            advantages.push_back(runningAdvantage);
        }
        std::reverse(advantages.begin(), advantages.end());
        
        // Update policy and value networks
        updateNetworks(advantages);
    }
    
    void updateNetworks(const std::vector<double>& advantages) {
        for (size_t t = 0; t < episodeStates.size() - 1; ++t) {
            std::vector<double> stateFeatures = extractFeatures(episodeStates[t]);
            RLActions& action = episodeActions[t];
            double value = episodeValues[t];
            double advantage = advantages[t];
            
            // Policy gradient update
            policyNet->update(stateFeatures, action, value, advantage);
            
            // Value network update
            double targetValue = returns[t] + gamma * episodeValues[t + 1] - episodeValues[t];
            valueNet->update(stateFeatures, targetValue);
        }
    }
    
    std::vector<double> extractFeatures(const RLState& state) {
        std::vector<double> features;
        
        // Simple feature extraction from state
        for (size_t i = 0; i < std::min(state.observations.size(), static_cast<size_t>(10)); ++i) {
            features.push_back(state.observations[i]);
        }
        
        // Add action features
        for (size_t i = 0; i < std::min(state.actions.size(), static_cast<size_t>(5)); ++i) {
            features.push_back(state.actions[i]);
        }
        
        // Add reward history
        for (size_t i = 0; i < std::min(state.rewards.size(), static_cast<size_t>(3)); ++i) {
            features.push_back(state.rewards[i]);
        }
        
        return features;
    }
    
    double getTotalReward() const {
        return totalReward;
    }
    
    int getEpisodeLength() const {
        return episodeLength;
    }
    
    std::string getRLStatus() const {
        std::stringstream ss;
        ss << "RL Status:\n";
        ss << "  Total Reward: " << totalReward << "\n";
        ss << "  Episode Length: " << episodeLength << "\n";
        ss << "  Discount Factor: " << gamma << "\n";
        ss << "  GAE Lambda: " << gaeLambda << "\n";
        return ss.str();
    }
    
    void saveModel(const Config& config) const {
        std::string filename = config.getOr("rl.model_file", "rl_model.bin");
        // Implementation for saving model
    }
    
    void loadModel(const Config& config) {
        std::string filename = config.getOr("rl.model_file", "rl_model.bin");
        // Implementation for loading model
    }
};

} // namespace nlm
