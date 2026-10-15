#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include "../world/Observation.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    float targetValue;
    float learningRate;
    std::vector<float> history;
    
    Impl() 
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , targetValue(0.0f)
        , learningRate(0.1f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->targetValue = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real implementation: compute reward based on observation features
    // For now, use a simple placeholder implementation
    const auto& data = observation.getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Simple reward based on average observation value
    float sum = 0.0f;
    for (float val : data) {
        sum += val;
    }
    float avg = sum / static_cast<float>(data.size());
    
    // Scale to [0, 1] range and apply some non-linearity
    float reward = std::tanh(avg * 0.5f);
    
    // Ensure consistency with current value
    return std::clamp(reward, -1.0f, 1.0f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

void Reward::setTargetValue(float target) {
    pImpl->targetValue = target;
}

void Reward::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

} // namespace nlm
