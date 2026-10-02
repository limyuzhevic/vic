#include "Reward.hpp"
#include "Environment.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    size_t historyCapacity;
    
    // Component weights
    float survivalWeight;
    float efficiencyWeight;
    float learningWeight;
    float explorationWeight;
    float overallWeight;
    
    // Statistics
    float minReward;
    float maxReward;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), historyCapacity(1000),
             survivalWeight(0.4f), efficiencyWeight(0.3f), learningWeight(0.2f), 
             explorationWeight(0.1f), overallWeight(1.0f),
             minReward(0.0f), maxReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
    addToHistory(value);
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    addToHistory(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->minReward = 0.0f;
    pImpl->maxReward = 0.0f;
}

float Reward::computeReward(const Observation& observation, std::shared_ptr<const Config> config) {
    if (!config) {
        return 0.0f;
    }
    
    // Extract observation data
    const auto& data = observation.getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Compute individual reward components
    float survivalReward = computeSurvivalComponent(observation, config);
    float efficiencyReward = computeEfficiencyComponent(observation, config);
    float learningReward = computeLearningComponent(observation, config);
    float explorationReward = computeExplorationComponent(observation, config);
    
    // Weighted combination using configured weights
    std::vector<float> components = {survivalReward, efficiencyReward, learningReward, explorationReward};
    std::vector<float> weights = {
        pImpl->survivalWeight,
        pImpl->efficiencyWeight,
        pImpl->learningWeight,
        pImpl->explorationWeight
    };
    
    float totalReward = weightedSum(components, weights);
    
    // Update statistics
    pImpl->currentValue = totalReward;
    addToHistory(totalReward);
    
    // Update min/max statistics
    if (totalReward < pImpl->minReward) {
        pImpl->minReward = totalReward;
    }
    if (totalReward > pImpl->maxReward) {
        pImpl->maxReward = totalReward;
    }
    
    pImpl->accumulatedReward += totalReward;
    
    return totalReward;
}

float Reward::computeSurvivalComponent(const Observation& observation, std::shared_ptr<const Config> config) {
    if (!config) return 0.0f;
    
    const auto& data = observation.getData();
    if (data.empty()) return 0.0f;
    
    // Simulate survival reward based on observation energy/health
    float energy = data[0]; // First element as energy
    float health = data.size() > 1 ? data[1] : energy;
    
    // Normalize to [0, 1] range
    float survivalValue = (energy + health) / 2.0f;
    return normalizeReward(survivalValue);
}

float Reward::computeEfficiencyComponent(const Observation& observation, std::shared_ptr<const Config> config) {
    if (!config) return 0.0f;
    
    const auto& data = observation.getData();
    if (data.size() < 3) return 0.0f;
    
    // Efficiency based on goal proximity and action success
    float progress = data[2]; // Third element as goal progress
    float actionSuccess = data.size() > 3 ? data[3] : progress;
    
    float efficiencyValue = (progress + actionSuccess) / 2.0f;
    return normalizeReward(efficiencyValue);
}

float Reward::computeLearningComponent(const Observation& observation, std::shared_ptr<const Config> config) {
    if (!config) return 0.0f;
    
    const auto& data = observation.getData();
    if (data.size() < 4) return 0.0f;
    
    // Learning based on novelty and adaptation
    float novelty = data[4]; // Fourth element as novelty
    float adaptation = data.size() > 5 ? data[5] : novelty;
    
    float learningValue = (novelty + adaptation) / 2.0f;
    return normalizeReward(learningValue);
}

float Reward::computeExplorationComponent(const Observation& observation, std::shared_ptr<const Config> config) {
    if (!config) return 0.0f;
    
    const auto& data = observation.getData();
    if (data.size() < 6) return 0.0f;
    
    // Exploration based on curiosity and seeking behavior
    float curiosity = data[6]; // Sixth element as curiosity
    float seeking = data.size() > 7 ? data[7] : curiosity;
    
    float explorationValue = (curiosity + seeking) / 2.0f;
    return normalizeReward(explorationValue);
}

float Reward::computeSurvivalReward(const Observation& observation, std::shared_ptr<const Config> config) {
    return computeSurvivalComponent(observation, config);
}

float Reward::computeEfficiencyReward(const Observation& observation, std::shared_ptr<const Config> config) {
    return computeEfficiencyComponent(observation, config);
}

float Reward::computeLearningReward(const Observation& observation, std::shared_ptr<const Config> config) {
    return computeLearningComponent(observation, config);
}

float Reward::computeExplorationReward(const Observation& observation, std::shared_ptr<const Config> config) {
    return computeExplorationComponent(observation, config);
}

float Reward::getAverageReward() const {
    if (pImpl->history.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
    return sum / pImpl->history.size();
}

float Reward::getMaxReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return pImpl->maxReward;
}

float Reward::getMinReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return pImpl->minReward;
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->minReward = 0.0f;
    pImpl->maxReward = 0.0f;
}

void Reward::setWeight(float weight) {
    pImpl->overallWeight = weight;
}

float Reward::getWeight() const {
    return pImpl->overallWeight;
}

void Reward::setSurvivalWeight(float weight) {
    pImpl->survivalWeight = std::max(0.0f, std::min(1.0f, weight));
    normalizeWeights();
}

void Reward::setEfficiencyWeight(float weight) {
    pImpl->efficiencyWeight = std::max(0.0f, std::min(1.0f, weight));
    normalizeWeights();
}

void Reward::setLearningWeight(float weight) {
    pImpl->learningWeight = std::max(0.0f, std::min(1.0f, weight));
    normalizeWeights();
}

void Reward::setExplorationWeight(float weight) {
    pImpl->explorationWeight = std::max(0.0f, std::min(1.0f, weight));
    normalizeWeights();
}

float Reward::getSurvivalWeight() const {
    return pImpl->survivalWeight;
}

float Reward::getEfficiencyWeight() const {
    return pImpl->efficiencyWeight;
}

float Reward::getLearningWeight() const {
    return pImpl->learningWeight;
}

float Reward::getExplorationWeight() const {
    return pImpl->explorationWeight;
}

void Reward::addToHistory(float reward) {
    if (pImpl->history.size() >= pImpl->historyCapacity) {
        pImpl->history.erase(pImpl->history.begin());
    }
    pImpl->history.push_back(reward);
}

size_t Reward::getHistorySize() const {
    return pImpl->history.size();
}

bool Reward::isHistoryFull() const {
    return pImpl->history.size() >= pImpl->historyCapacity;
}

void Reward::setHistoryCapacity(size_t capacity) {
    pImpl->historyCapacity = capacity;
    if (pImpl->history.size() > capacity) {
        pImpl->history.erase(pImpl->history.begin(), 
                           pImpl->history.begin() + (pImpl->history.size() - capacity));
    }
}

float Reward::normalizeReward(float reward, float min, float max) {
    if (max <= min) return 0.0f;
    float normalized = (reward - min) / (max - min);
    return std::max(0.0f, std::min(1.0f, normalized));
}

float Reward::denormalizeReward(float normalizedReward, float min, float max) {
    if (max <= min) return 0.0f;
    return min + normalizedReward * (max - min);
}

std::string Reward::getRewardAnalysis() const {
    std::ostringstream oss;
    oss << "Reward Analysis:" << std::endl;
    oss << "  Current value: " << pImpl->currentValue << std::endl;
    oss << "  Accumulated: " << pImpl->accumulatedReward << std::endl;
    oss << "  Average: " << getAverageReward() << std::endl;
    oss << "  Minimum: " << pImpl->minReward << std::endl;
    oss << "  Maximum: " << pImpl->maxReward << std::endl;
    oss << "  History size: " << pImpl->history.size() << std::endl;
    oss << "  Component weights: " << std::endl;
    oss << "    Survival: " << pImpl->survivalWeight << std::endl;
    oss << "    Efficiency: " << pImpl->efficiencyWeight << std::endl;
    oss << "    Learning: " << pImpl->learningWeight << std::endl;
    oss << "    Exploration: " << pImpl->explorationWeight << std::endl;
    
    return oss.str();
}

std::vector<float> Reward::getNormalizedHistory() const {
    std::vector<float> normalized;
    if (pImpl->history.empty()) return normalized;
    
    float min = pImpl->minReward;
    float max = pImpl->maxReward;
    if (max <= min) max = min + 1.0f;
    
    for (float reward : pImpl->history) {
        normalized.push_back(normalizeReward(reward, min, max));
    }
    
    return normalized;
}

float Reward::weightedSum(const std::vector<float>& components, const std::vector<float>& weights) {
    if (components.size() != weights.size() || components.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    float weightSum = 0.0f;
    
    for (size_t i = 0; i < components.size(); ++i) {
        sum += components[i] * weights[i];
        weightSum += weights[i];
    }
    
    if (weightSum > 0.0f) {
        return sum / weightSum;
    }
    
    return 0.0f;
}

float Reward::maxReward(const std::vector<float>& components) {
    if (components.empty()) return 0.0f;
    return *std::max_element(components.begin(), components.end());
}

float Reward::minReward(const std::vector<float>& components) {
    if (components.empty()) return 0.0f;
    return *std::min_element(components.begin(), components.end());
}

void Reward::normalizeWeights() {
    float totalWeight = pImpl->survivalWeight + pImpl->efficiencyWeight + 
                       pImpl->learningWeight + pImpl->explorationWeight;
    
    if (totalWeight > 0.0f) {
        pImpl->survivalWeight /= totalWeight;
        pImpl->efficiencyWeight /= totalWeight;
        pImpl->learningWeight /= totalWeight;
        pImpl->explorationWeight /= totalWeight;
    }
}

} // namespace nlm