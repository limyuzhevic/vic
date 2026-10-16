#include "Reward.hpp"
#include "../world/Observation.hpp"
#include "../environment/Observation.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <ctime>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    std::vector<double> historyTimestamps;
    std::vector<RewardComponent> components;
    double lastUpdateTime;
    float expectedReward;
    float predictionError;
    RewardType dominantType;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             lastUpdateTime(0.0), expectedReward(0.0f), 
             predictionError(0.0f), dominantType(RewardType::Mixed) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->updateHistory(delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->expectedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation, RewardType source) {
    // Get current timestamp
    double currentTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    float totalReward = 0.0f;
    pImpl->components.clear();
    
    // Extract observation data
    float observationValue = 0.0f;
    size_t dimensions = 0;
    
    // Try different Observation types for data extraction
    try {
        // Try world::Observation first
        const auto* worldObs = dynamic_cast<const nlm::world::Observation*>(&observation);
        if (worldObs) {
            const auto& data = worldObs->getData();
            if (!data.empty()) {
                observationValue = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
                dimensions = data.size();
            }
        }
    } catch (...) {}
    
    try {
        // Try environment::Observation
        const auto* envObs = dynamic_cast<const nlm::environment::Observation*>(&observation);
        if (envObs) {
            const SensoryInput* sensoryInput = envObs->getSensoryInput();
            if (sensoryInput) {
                const auto& data = sensoryInput->getData();
                if (!data.empty()) {
                    observationValue = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
                    dimensions = data.size();
                }
            }
        }
    } catch (...) {}
    
    // Base reward from observation value (normalized)
    float baseReward = normalizeReward(observationValue);
    
    // Apply reward type specific computation
    switch (source) {
        case RewardType::Extrinsic:
            totalReward = baseReward * 0.8f;  // External rewards are weighted
            break;
        case RewardType::Intrinsic:
            totalReward = baseReward * 0.6f + computeExplorationBonus();  // Add exploration bonus
            break;
        case RewardType::Social:
            totalReward = baseReward * 0.7f;  // Social rewards are moderate
            break;
        case RewardType::Novelty:
            totalReward = baseReward * 0.9f;  // Novelty rewards are slightly higher
            break;
        case RewardType::PredictionError:
            // Prediction error will be computed separately
            totalReward = pImpl->predictionError;
            break;
        case RewardType::Mixed:
            totalReward = (baseReward * 0.5f) + computeExplorationBonus() * 0.3f;
            break;
    }
    
    // Add reward component
    pImpl->components.push_back(RewardComponent(source, totalReward, 1.0f, 0.01f, currentTime));
    
    // Update current value and history
    pImpl->currentValue = totalReward;
    pImpl->updateHistory(totalReward);
    pImpl->lastUpdateTime = currentTime;
    pImpl->dominantType = source;
    
    return totalReward;
}

float Reward::computePredictionError(float expected, float actual) {
    float error = actual - expected;
    pImpl->predictionError = error;
    pImpl->expectedReward = expected;
    
    // Update history with prediction error
    pImpl->updateHistory(error);
    
    // Add prediction error component
    double currentTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    pImpl->components.push_back(RewardComponent(RewardType::PredictionError, error, 1.0f, 0.05f, currentTime));
    
    return error;
}

void Reward::updateHistory(float reward) {
    double currentTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    pImpl->history.push_back(reward);
    pImpl->historyTimestamps.push_back(currentTime);
    
    // Keep history size bounded (last 1000 entries)
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
        pImpl->historyTimestamps.erase(pImpl->historyTimestamps.begin());
    }
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

const std::vector<double>& Reward::getHistoryTimestamps() const {
    return pImpl->historyTimestamps;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->historyTimestamps.clear();
}

float Reward::normalizeReward(float reward, float minExpected, float maxExpected) {
    // Apply sigmoid-like normalization for better learning
    float normalized = (reward - minExpected) / (maxExpected - minExpected);
    
    // Apply tanh for bounded output [-1, 1]
    normalized = std::tanh(normalized);
    
    // Scale to [-0.5, 0.5] for more stable learning
    return normalized * 0.5f;
}

bool Reward::isSignificant(float reward, float threshold) const {
    return std::abs(reward) > threshold;
}

void Reward::applyDecay(float decayRate) {
    // Apply exponential decay to accumulated reward
    pImpl->currentValue *= (1.0f - decayRate);
    pImpl->accumulatedReward *= (1.0f - decayRate);
    
    // Decay components
    for (auto& component : pImpl->components) {
        component.value *= (1.0f - component.decayRate);
    }
}

void Reward::modulateWithNeuromodulator(const Neuromodulator& signal) {
    // Apply neuromodulatory effects to reward
    if (signal.getName() == std::string("Dopamine")) {
        // Dopamine enhances reward salience
        float dopamineLevel = signal.getLevel();
        pImpl->currentValue *= (1.0f + dopamineLevel * 0.5f);
    } else if (signal.getName() == std::string("Acetylcholine")) {
        // Acetylcholine modulates attention to reward
        float achLevel = signal.getLevel();
        // Scale reward based on attention
        pImpl->currentValue *= (1.0f + achLevel * 0.3f);
    } else if (signal.getName() == std::string("Norepinephrine")) {
        // Norepinephrine modulates arousal and reward responsiveness
        float neLevel = signal.getLevel();
        pImpl->currentValue *= (1.0f + neLevel * 0.2f);
    } else if (signal.getName() == std::string("5-HT")) {
        // Serotonin modulates reward for social behavior
        float serotoninLevel = signal.getLevel();
        pImpl->currentValue *= (1.0f - serotoninLevel * 0.1f);  // Reduces reward for social
    }
}

float Reward::computeExplorationBonus(float uncertainty, float explorationRate) {
    // Compute exploration bonus based on uncertainty and exploration rate
    float bonus = 0.0f;
    
    if (uncertainty > 0.5f) {
        // High uncertainty encourages exploration
        bonus = (uncertainty - 0.5f) * 2.0f * explorationRate;
    }
    
    // Add temporal component based on time since last reward
    double currentTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    double timeSinceUpdate = currentTime - pImpl->lastUpdateTime;
    
    if (timeSinceUpdate > 1.0) {  // If it's been more than 1 second
        bonus += explorationRate * 0.5f * std::min(1.0, timeSinceUpdate / 10.0);
    }
    
    return std::max(0.0f, bonus);  // Ensure bonus is non-negative
}

void Reward::setRewardType(RewardType type, float weight) {
    // Update or add a reward type component
    bool found = false;
    for (auto& component : pImpl->components) {
        if (component.type == type) {
            component.weight = weight;
            component.timestamp = std::chrono::duration<double>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            found = true;
            break;
        }
    }
    
    if (!found) {
        double currentTime = std::chrono::duration<double>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        pImpl->components.push_back(RewardComponent(type, 0.0f, weight, 0.01f, currentTime));
    }
}

RewardType Reward::getDominantRewardType() const {
    return pImpl->dominantType;
}

const std::vector<RewardComponent>& Reward::getComponents() const {
    return pImpl->components;
}

float Reward::getTotalWeightedReward() const {
    if (pImpl->components.empty()) {
        return 0.0f;
    }
    
    float total = 0.0f;
    for (const auto& component : pImpl->components) {
        total += component.value * component.weight;
    }
    
    return total / pImpl->components.size();
}

} // namespace nlm
