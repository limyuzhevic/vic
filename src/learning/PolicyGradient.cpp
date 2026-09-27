#include "PolicyGradient.hpp"
#include "../core/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>
#include <random>
#include <tuple>
#include <unordered_map>

namespace nlm {

struct PolicyGradient::Impl {
    class Brain* brain;
    size_t actionSpaceSize;
    float learningRate;
    float discountFactor;
    float explorationRate;
    float temperature;
    
    // Policy representation (softmax over action values)
    std::unordered_map<size_t, std::vector<float>> actionValues;
    std::unordered_map<size_t, std::vector<float>> policyGradients;
    
    // Value function (state value estimates)
    std::unordered_map<size_t, std::vector<float>> stateValues;
    
    // Experience replay memory
    std::vector<std::tuple<std::vector<float>, size_t, float, std::vector<float>>> experienceMemory;
    size_t maxMemorySize;
    
    // Learning state
    bool enabled;
    float policyGradientMagnitude;
    
    // Exploration parameters
    std::string explorationStrategy;
    float epsilonGreedy;
    float boltzmannTemp;
    float ouMu;
    float ouSigma;
    
    // Statistics
    float totalReward;
    size_t totalSteps;
    std::vector<float> valueHistory;
    
    Impl() : brain(nullptr), actionSpaceSize(0), learningRate(0.01f),
             discountFactor(0.95f), explorationRate(0.1f), temperature(1.0f),
             enabled(true), policyGradientMagnitude(0.0f), maxMemorySize(10000),
             epsilonGreedy(0.1f), boltzmannTemp(1.0f), ouMu(0.0f), ouSigma(0.1f),
             totalReward(0.0f), totalSteps(0) {}
};

PolicyGradient::PolicyGradient() : pImpl(std::make_unique<Impl>()) {}

PolicyGradient::~PolicyGradient() = default;

void PolicyGradient::initialize(class Brain* brain, size_t actionSpaceSize,
                                float learningRate, float discountFactor,
                                float explorationRate) {
    pImpl->brain = brain;
    pImpl->actionSpaceSize = actionSpaceSize;
    pImpl->learningRate = learningRate;
    pImpl->discountFactor = discountFactor;
    pImpl->explorationRate = explorationRate;
    pImpl->temperature = 1.0f;
    
    // Initialize state action value function
    pImpl->actionValues.clear();
    pImpl->stateValues.clear();
    pImpl->policyGradients.clear();
    
    // Set default exploration strategy
    pImpl->explorationStrategy = "epsilon_greedy";
    
    // Initialize experience memory
    pImpl->experienceMemory.reserve(pImpl->maxMemorySize);
    
    NLM_LOG_INFO("Policy gradient system initialized");
    NLM_LOG_INFO("Action space size: " + std::to_string(actionSpaceSize));
    NLM_LOG_INFO("Learning rate: " + std::to_string(learningRate));
    NLM_LOG_INFO("Discount factor: " + std::to_string(discountFactor));
    NLM_LOG_INFO("Exploration rate: " + std::to_string(explorationRate));
}

size_t PolicyGradient::selectAction(const std::vector<float>& state, float temperature) {
    if (!pImpl->enabled) {
        return 0; // Default action
    }
    
    // Get action values for current state
    auto it = pImpl->actionValues.find(state.size());
    if (it == pImpl->actionValues.end()) {
        // Initialize action values for this state
        std::vector<float> actionVals(pImpl->actionSpaceSize, 0.0f);
        pImpl->actionValues[state.size()] = actionVals;
        it = pImpl->actionValues.find(state.size());
    }
    
    const std::vector<float>& actionValues = it->second;
    
    // Select action based on exploration strategy
    size_t selectedAction = 0;
    
    if (pImpl->explorationStrategy == "epsilon_greedy") {
        // Epsilon-greedy exploration
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        float randVal = dis(gen);
        if (randVal < pImpl->epsilonGreedy) {
            // Explore: select random action
            std::uniform_int_distribution<size_t> actionDist(0, pImpl->actionSpaceSize - 1);
            selectedAction = actionDist(gen);
        } else {
            // Exploit: select best action
            selectedAction = std::max_element(actionValues.begin(), actionValues.end()) - actionValues.begin();
        }
    } else if (pImpl->explorationStrategy == "boltzmann") {
        // Boltzmann exploration
        std::vector<float> probabilities;
        float sumExp = 0.0f;
        
        for (float value : actionValues) {
            float expVal = std::exp(value / temperature);
            probabilities.push_back(expVal);
            sumExp += expVal;
        }
        
        if (sumExp > 0.0f) {
            // Normalize probabilities
            for (float& prob : probabilities) {
                prob /= sumExp;
            }
            
            // Sample from distribution
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(0.0f, 1.0f);
            
            float randVal = dis(gen);
            float cumulative = 0.0f;
            
            for (size_t i = 0; i < probabilities.size(); ++i) {
                cumulative += probabilities[i];n                if (randVal <= cumulative) {
                    selectedAction = i;
                    break;
                }
            }
        }
    } else {
        // Default: epsilon-greedy
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        float randVal = dis(gen);
        if (randVal < pImpl->explorationRate) {
            std::uniform_int_distribution<size_t> actionDist(0, pImpl->actionSpaceSize - 1);
            selectedAction = actionDist(gen);
        } else {
            selectedAction = std::max_element(actionValues.begin(), actionValues.end()) - actionValues.begin();
        }
    }
    
    return selectedAction;
}

void PolicyGradient::updatePolicy(const std::vector<float>& state, size_t action,
                                  float reward, const std::vector<float>& nextState) {
    if (!pImpl->enabled) {
        return;
    }
    
    // Add experience to memory
    addExperience(state, action, reward, nextState);
    
    // Update value functions
    updateValueFunctions(state, action, reward, nextState);
    
    // Update policy gradients
    updatePolicyGradients(state, action, reward);
    
    // Update statistics
    pImpl->totalReward += reward;
    pImpl->totalSteps++;
    pImpl->valueHistory.push_back(reward);
}

std::vector<float> PolicyGradient::getActionProbabilities(const std::vector<float>& state) const {
    std::vector<float> probabilities;
    
    auto it = pImpl->actionValues.find(state.size());
    if (it == pImpl->actionValues.end()) {
        // Return uniform distribution if state not seen before
        probabilities.assign(pImpl->actionSpaceSize, 1.0f / pImpl->actionSpaceSize);
        return probabilities;
    }
    
    const std::vector<float>& actionValues = it->second;
    
    // Apply softmax to get probabilities
    float maxValue = *std::max_element(actionValues.begin(), actionValues.end());
    float sumExp = 0.0f;
    
    for (float value : actionValues) {
        sumExp += std::exp((value - maxValue) / pImpl->temperature);
    }
    
    for (float value : actionValues) {
        float prob = std::exp((value - maxValue) / pImpl->temperature) / sumExp;
        probabilities.push_back(prob);
    }
    
    return probabilities;
}

float PolicyGradient::getValueEstimate(const std::vector<float>& state) const {
    auto it = pImpl->stateValues.find(state.size());
    if (it == pImpl->stateValues.end()) {
        return 0.0f; // Default value
    }
    
    const std::vector<float>& values = it->second;
    if (values.empty()) {
        return 0.0f;
    }
    
    // Return average value for this state
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return sum / values.size();
}

float PolicyGradient::getPolicyGradientMagnitude() const {
    return pImpl->policyGradientMagnitude;
}

float PolicyGradient::getExplorationRate() const {
    return pImpl->explorationRate;
}

const std::vector<std::tuple<std::vector<float>, size_t, float, std::vector<float>>>&
PolicyGradient::getExperienceMemory() const {
    return pImpl->experienceMemory;
}

void PolicyGradient::addExperience(const std::vector<float>& state, size_t action,
                                    float reward, const std::vector<float>& nextState) {
    // Add to experience memory
    pImpl->experienceMemory.emplace_back(state, action, reward, nextState);
    
    // Trim memory if too large
    if (pImpl->experienceMemory.size() > pImpl->maxMemorySize) {
        pImpl->experienceMemory.erase(pImpl->experienceMemory.begin());
    }
}

void PolicyGradient::clearExperienceMemory() {
    pImpl->experienceMemory.clear();
}

void PolicyGradient::enable(bool enable) {
    pImpl->enabled = enable;
}

bool PolicyGradient::isEnabled() const {
    return pImpl->enabled;
}

void PolicyGradient::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

void PolicyGradient::setDiscountFactor(float factor) {
    pImpl->discountFactor = factor;
}

void PolicyGradient::setExplorationRate(float rate) {
    pImpl->explorationRate = rate;
}

void PolicyGradient::setTemperature(float temp) {
    pImpl->temperature = temp;
}

void PolicyGradient::setExplorationStrategy(const std::string& strategy) {
    pImpl->explorationStrategy = strategy;
}

void PolicyGradient::setEpsilonGreedy(float epsilon) {
    pImpl->epsilonGreedy = epsilon;
}

void PolicyGradient::setBoltzmannExploration(float temp) {
    pImpl->boltzmannTemp = temp;
}

void PolicyGradient::setOUExploration(float mu, float sigma) {
    pImpl->ouMu = mu;
    pImpl->ouSigma = sigma;
}

float PolicyGradient::getEntropy() const {
    float totalEntropy = 0.0f;
    
    for (const auto& entry : pImpl->actionValues) {
        const std::vector<float>& actionValues = entry.second;
        
        // Calculate softmax entropy
        float maxValue = *std::max_element(actionValues.begin(), actionValues.end());
        float sumExp = 0.0f;
        
        for (float value : actionValues) {
            sumExp += std::exp((value - maxValue) / pImpl->temperature);
        }
        
        float entropy = 0.0f;
        for (float value : actionValues) {
            float prob = std::exp((value - maxValue) / pImpl->temperature) / sumExp;
            if (prob > 0.0f) {
                entropy -= prob * std::log(prob);
            }
        }
        
        totalEntropy += entropy;
    }
    
    return totalEntropy / pImpl->actionValues.size();
}

size_t PolicyGradient::getActionCount(const std::vector<float>& state) const {
    auto it = pImpl->actionValues.find(state.size());
    if (it == pImpl->actionValues.end()) {
        return pImpl->actionSpaceSize;
    }
    return pImpl->actionValues.at(state.size()).size();
}

float PolicyGradient::getMaxValue() const {
    float maxVal = std::numeric_limits<float>::lowest();
    
    for (const auto& entry : pImpl->stateValues) {
        const std::vector<float>& values = entry.second;
        if (!values.empty()) {
            float maxEntry = *std::max_element(values.begin(), values.end());
            maxVal = std::max(maxVal, maxEntry);
        }
    }
    
    return maxVal;
}

float PolicyGradient::getAverageValue() const {
    if (pImpl->stateValues.empty()) {
        return 0.0f;
    }
    
    float total = 0.0f;
    size_t count = 0;
    
    for (const auto& entry : pImpl->stateValues) {
        const std::vector<float>& values = entry.second;
        if (!values.empty()) {
            total += std::accumulate(values.begin(), values.end(), 0.0f);
            count += values.size();
        }
    }
    
    return count > 0 ? total / count : 0.0f;
}

void PolicyGradient::trainExperienceReplay(size_t batchSize) {
    if (pImpl->experienceMemory.size() < batchSize) {
        batchSize = pImpl->experienceMemory.size();
    }
    
    // Sample batch from experience memory
    std::vector<std::tuple<std::vector<float>, size_t, float, std::vector<float>>> batch;
    
    if (pImpl->experienceMemory.size() > batchSize) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, pImpl->experienceMemory.size() - 1);
        
        std::unordered_set<size_t> selectedIndices;
        while (selectedIndices.size() < batchSize) {
            size_t idx = dis(gen);
            selectedIndices.insert(idx);
        }
        
        for (size_t idx : selectedIndices) {
            batch.push_back(pImpl->experienceMemory[idx]);
        }
    } else {
        batch = pImpl->experienceMemory;
    }
    
    // Train on batch
    updatePolicyGradient(batch);
}

void PolicyGradient::updatePolicyGradient(const std::vector<std::tuple<std::vector<float>, size_t, float, std::vector<float>>>& batch) {
    float totalGradient = 0.0f;
    
    for (const auto& experience : batch) {
        const std::vector<float>& state = std::get<0>(experience);
        size_t action = std::get<1>(experience);
        float reward = std::get<2>(experience);
        const std::vector<float>& nextState = std::get<3>(experience);
        
        // Calculate policy gradient for this experience
        float gradient = calculatePolicyGradient(state, action, reward, nextState);
        totalGradient += std::abs(gradient);
        
        // Update action values
        updateActionValues(state, action, reward, nextState);
    }
    
    pImpl->policyGradientMagnitude = totalGradient / batch.size();
}

void PolicyGradient::updateValueFunctions(const std::vector<float>& state, size_t action,
                                          float reward, const std::vector<float>& nextState) {
    // Update state value function using TD error
    float currentValue = getValueEstimate(state);
    float nextValue = getValueEstimate(nextState);
    
    // Calculate TD target
    float tdTarget = reward + pImpl->discountFactor * nextValue;
    
    // Calculate TD error
    float tdError = tdTarget - currentValue;
    
    // Update state values
    auto it = pImpl->stateValues.find(state.size());
    if (it == pImpl->stateValues.end()) {
        pImpl->stateValues[state.size()] = std::vector<float>(1, currentValue);
        it = pImpl->stateValues.find(state.size());
    }
    
    std::vector<float>& values = it->second;
    if (!values.empty()) {
        // Update the most recent value estimate
        values.back() += pImpl->learningRate * tdError;
    }
}

void PolicyGradient::updateActionValues(const std::vector<float>& state, size_t action,
                                        float reward, const std::vector<float>& nextState) {
    // Update action values using policy gradient
    auto it = pImpl->actionValues.find(state.size());
    if (it == pImpl->actionValues.end()) {
        return;
    }
    
    std::vector<float>& actionValues = it->second;
    
    if (action >= actionValues.size()) {
        return;
    }
    
    // Calculate advantage (reward - baseline)
    float baseline = getValueEstimate(state);
    float advantage = reward - baseline;
    
    // Update action value using policy gradient rule
    actionValues[action] += pImpl->learningRate * advantage;
}

float PolicyGradient::calculatePolicyGradient(const std::vector<float>& state, size_t action,
                                              float reward, const std::vector<float>& nextState) const {
    // Calculate policy gradient for a single experience
    std::vector<float> actionProbs = getActionProbabilities(state);
    
    if (action >= actionProbs.size()) {
        return 0.0f;
    }
    
    float prob = actionProbs[action];
    float logProb = prob > 0.0f ? std::log(prob) : 0.0f;
    
    // Policy gradient with baseline
    float baseline = getValueEstimate(state);
    float advantage = reward - baseline;
    
    // Gradient = advantage * log(probability of action)
    float gradient = advantage * logProb;
    
    return gradient;
}

float PolicyGradient::getPolicyPerformance() const {
    if (pImpl->valueHistory.empty()) {
        return 0.0f;
    }
    
    // Calculate average performance
    float sum = std::accumulate(pImpl->valueHistory.begin(), pImpl->valueHistory.end(), 0.0f);
    return sum / pImpl->valueHistory.size();
}

} // namespace nlm