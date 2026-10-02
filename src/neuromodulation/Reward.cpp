#include "Reward.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <mutex>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    static constexpr size_t maxHistorySize = 1000;
    mutable std::mutex historyMutex;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), history() {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return getValue();
}

void Reward::setLevel(float level) {
    setValue(level);
}

float Reward::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real reward-modulated plasticity factor
    // PLACEHOLDER: Higher reward increases plasticity
    return 0.5f + 0.5f * getValue();
}

void Reward::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real reward dynamics
    // PLACEHOLDER: Decay towards baseline
    const float baseline = 0.0f;
    setValue(std::max(baseline, getValue() - 0.1f * static_cast<float>(dt)));
}

float Reward::computeReward(const Observation& observation, std::shared_ptr<const Config> config) {
    if (!observation.isValid()) {
        NLM_LOG_INFO_WITH_CONTEXT("Invalid observation for reward computation", "Reward::computeReward");
        return 0.0f;
    }
    
    const auto& data = observation.getData();
    if (data.empty()) return 0.0f;
    
    // Component calculations with performance optimization
    float survivalReward = computeSurvivalReward(observation, config);
    float efficiencyReward = computeEfficiencyReward(observation, config);
    float learningReward = computeLearningReward(observation, config);
    float explorationReward = computeExplorationReward(observation, config);
    
    // Weighted combination (optimized to minimize branches)
    float totalReward = survivalReward * 0.4f + 
                       efficiencyReward * 0.3f + 
                       learningReward * 0.2f + 
                       explorationReward * 0.1f;
    
    // Thread-safe update with history management
    {
        std::lock_guard<std::mutex> lock(pImpl->historyMutex);
        if (pImpl->history.size() >= Impl::maxHistorySize) {
            pImpl->history.erase(pImpl->history.begin());
        }
        pImpl->history.push_back(totalReward);
        pImpl->currentValue = totalReward;
        pImpl->accumulatedReward += totalReward;
    }
    
    return totalReward;
}

float Reward::computeSurvivalReward(const Observation& observation, std::shared_ptr<const Config> config) {
    // Placeholder implementation
    // TODO PHASE 2: Implement real survival reward based on energy, health, safety
    const auto& data = observation.getData();
    if (data.empty()) return 0.0f;
    
    // Simple placeholder based on observation data
    float sum = 0.0f;
    for (float val : data) {
        sum += std::max(0.0f, val);
    }
    return std::min(1.0f, sum / static_cast<float>(data.size()) * 0.5f);
}

float Reward::computeEfficiencyReward(const Observation& observation, std::shared_ptr<const Config> config) {
    // Placeholder implementation
    // TODO PHASE 2: Implement real efficiency reward based on action success, goal progress
    const auto& data = observation.getData();
    if (data.empty()) return 0.0f;
    
    // Simple placeholder based on observation data
    float positiveValues = 0.0f;
    for (float val : data) {
        if (val > 0.0f) positiveValues += val;
    }
    return std::min(1.0f, positiveValues / static_cast<float>(data.size()) * 0.8f);
}

float Reward::computeLearningReward(const Observation& observation, std::shared_ptr<const Config> config) {
    // Placeholder implementation
    // TODO PHASE 2: Implement real learning reward based on novelty, adaptation
    const auto& data = observation.getData();
    if (data.empty()) return 0.0f;
    
    // Simple placeholder based on observation data variance
    float mean = 0.0f;
    for (float val : data) mean += val;
    mean /= static_cast<float>(data.size());
    
    float variance = 0.0f;
    for (float val : data) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= static_cast<float>(data.size());
    
    return std::min(1.0f, std::sqrt(variance) * 2.0f);
}

float Reward::computeExplorationReward(const Observation& observation, std::shared_ptr<const Config> config) {
    // Placeholder implementation
    // TODO PHASE 2: Implement real exploration reward based on curiosity, seeking
    const auto& data = observation.getData();
    if (data.empty()) return 0.0f;
    
    // Simple placeholder based on observation data extremes
    float min = data[0];
    float max = data[0];
    for (float val : data) {
        min = std::min(min, val);
        max = std::max(max, val);
    }
    float range = max - min;
    
    return std::min(1.0f, range * 2.0f);
}

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
