#include "Reward.hpp"
#include "../environment/Observation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    float level;
    float accumulatedReward;
    float baselineReward;
    float peakReward;
    float decayRate;
    float learningRate;
    std::vector<float> history;
    std::vector<float> predictionErrors;
    
    Impl() 
        : level(0.0f)
        , accumulatedReward(0.0f)
        , baselineReward(0.0f)
        , peakReward(1.0f)
        , decayRate(0.1f)
        , learningRate(0.3f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return pImpl->level;
}

void Reward::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Reward::getPlasticityFactor() const {
    // Reward modulates plasticity - higher reward increases plasticity
    // Based on dopamine-like effects but specifically for reward
    return 0.5f + 0.5f * pImpl->level;
}

void Reward::update(TimestepDuration dt) {
    // Reward decays towards baseline over time (habituation)
    pImpl->level = std::max(pImpl->baselineReward, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Reward::signalReward(float reward) {
    // Signal a reward occurrence
    float normalizedReward = std::clamp(reward, 0.0f, 1.0f);
    
    // Update peak reward
    pImpl->peakReward = std::max(pImpl->peakReward, normalizedReward);
    
    // Apply learning rate to current level
    pImpl->level = pImpl->level * (1.0f - pImpl->learningRate) + 
                  normalizedReward * pImpl->learningRate;
    
    // Accumulate total reward
    pImpl->accumulatedReward += reward;
    
    // Store in history
    pImpl->history.push_back(reward);
    
    NLM_LOG_INFO("Reward signaled: " + std::to_string(reward) + ", current level: " + 
                std::to_string(pImpl->level));
}

void Reward::signalRewardPredictionError(float error) {
    // Signal reward prediction error (difference between expected and actual reward)
    pImpl->predictionErrors.push_back(error);
    
    // Large positive error (better than expected) increases reward level
    // Large negative error (worse than expected) decreases reward level
    float rewardAdjustment = error * pImpl->learningRate * 0.5f;
    pImpl->level = std::clamp(pImpl->level + rewardAdjustment, 0.0f, 1.0f);
    
    NLM_LOG_INFO("Reward prediction error: " + std::to_string(error) + 
                ", adjusted level: " + std::to_string(pImpl->level));
}

void Reward::accumulateReward(float delta) {
    // Accumulate reward over time (for delayed rewards)
    pImpl->accumulatedReward += delta;
    pImpl->level = std::clamp(pImpl->level + delta * 0.01f, 0.0f, 1.0f);
    pImpl->history.push_back(delta);
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

void Reward::resetAccumulatedReward() {
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // Placeholder: Extract sensory input and compute simple reward based on intensity
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) return 0.0f;
    
    const auto& data = sensoryInput->getData();
    if (data.empty()) return 0.0f;
    
    // Simple reward computation based on average sensory input intensity
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float average = sum / static_cast<float>(data.size());
    
    // Normalize to [0, 1] range
    float normalized = std::clamp((average + 10.0f) / 20.0f, 0.0f, 1.0f);
    
    NLM_LOG_DEBUG("Computed reward from observation: " + std::to_string(normalized));
    return normalized;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->predictionErrors.clear();
}

float Reward::getAverageReward() const {
    if (pImpl->history.empty()) return 0.0f;
    float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
    return sum / static_cast<float>(pImpl->history.size());
}

float Reward::getMaxReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return *std::max_element(pImpl->history.begin(), pImpl->history.end());
}

float Reward::getMinReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return *std::min_element(pImpl->history.begin(), pImpl->history.end());
}

} // namespace nlm