#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
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
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation from observation
    // Computes reward based on sensory input characteristics:
    // - High magnitude signals indicate important events
    // - Novel or unexpected patterns yield positive reward
    // - Consistent signals maintain baseline reward
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput || !sensoryInput->isValid()) {
        return 0.0f;
    }
    
    const std::vector<float>& data = sensoryInput->getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Calculate reward based on data characteristics
    float sum = 0.0f;
    float sumSquares = 0.0f;
    float maxVal = data[0];
    float minVal = data[0];
    
    for (float val : data) {
        sum += val;
        sumSquares += val * val;
        if (val > maxVal) maxVal = val;
        if (val < minVal) minVal = val;
    }
    
    float mean = sum / data.size();
    float variance = (sumSquares / data.size()) - (mean * mean);
    float stdDev = std::sqrt(std::max(0.0f, variance));
    
    // Novelty reward: higher standard deviation indicates novel stimuli
    float noveltyReward = std::min(1.0f, stdDev * 10.0f);
    
    // Magnitude reward: larger absolute values indicate important events
    float magnitudeReward = std::min(1.0f, (std::abs(maxVal) + std::abs(minVal)) / 2.0f);
    
    // Combined reward signal
    float reward = (noveltyReward * 0.6f) + (magnitudeReward * 0.4f);
    
    // Add to history
    pImpl->history.push_back(reward);
    
    // Keep history manageable
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
