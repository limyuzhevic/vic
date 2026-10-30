#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float baselineReward;
    float noveltyReward;
    float predictionErrorReward;
    float temporalDiscount;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), baselineReward(0.1f), 
             noveltyReward(0.3f), predictionErrorReward(0.4f), temporalDiscount(0.95f) {}
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
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const auto& data = sensoryInput->getData();
    
    // Compute multiple reward components
    float baseline = computeBaselineReward(data);
    float novelty = computeNoveltyReward(data);
    float predictionError = computePredictionErrorReward(data);
    
    // Weighted combination of reward components
    float totalReward = pImpl->baselineReward * baseline +
                       pImpl->noveltyReward * novelty +
                       pImpl->predictionErrorReward * predictionError;
    
    // Apply temporal discount
    pImpl->currentValue = totalReward * pImpl->temporalDiscount;
    pImpl->accumulatedReward += pImpl->currentValue;
    pImpl->history.push_back(pImpl->currentValue);
    
    NLM_LOG_INFO("Reward computed: Baseline=" + std::to_string(baseline) +
                " Novelty=" + std::to_string(novelty) +
                " PredictionError=" + std::to_string(predictionError) +
                " Total=" + std::to_string(totalReward));
    
    return pImpl->currentValue;
}

float Reward::computeBaselineReward(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    // Baseline reward based on proximity to expected values
    // For now, use simple variance - lower variance = higher baseline reward
    float mean = 0.0f;
    for (float val : data) mean += val;
    mean /= data.size();
    
    float variance = 0.0f;
    for (float val : data) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= data.size();
    
    // Lower variance (more predictable) gives higher baseline reward
    return std::max(0.0f, 1.0f - variance * 10.0f);
}

float Reward::computeNoveltyReward(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    // Novelty reward based on pattern deviation from historical patterns
    // For now, use magnitude of data values as novelty indicator
    float novelty = 0.0f;
    for (float val : data) {
        novelty += std::abs(val);
    }
    novelty /= data.size();
    
    // Normalize to [0, 1] range
    return std::min(1.0f, novelty * 2.0f);
}

float Reward::computePredictionErrorReward(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    // Prediction error reward based on magnitude of prediction errors
    // Higher prediction errors (better learning) give higher reward
    float totalError = 0.0f;
    for (float val : data) {
        totalError += std::abs(val);
    }
    
    // Scale to [0, 1] range
    return std::min(1.0f, totalError * 0.5f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm

