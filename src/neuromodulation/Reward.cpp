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

void Reward::applyObservation(const Observation& observation) {
    // Compute reward from the current observation and update state
    float computedReward = computeReward(observation);
    add(computedReward);
    
    // Add to history for later analysis
    pImpl->history.push_back(computedReward);
    
    // Keep history manageable (limit to last 1000 rewards)
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Reward::getAverageReward() const {
    if (pImpl->history.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float reward : pImpl->history) {
        sum += reward;
    }
    return sum / static_cast<float>(pImpl->history.size());
}

float Reward::getMaxReward() const {
    if (pImpl->history.empty()) return 0.0f;
    
    float max = pImpl->history[0];
    for (float reward : pImpl->history) {
        if (reward > max) max = reward;
    }
    return max;
}

float Reward::getMinReward() const {
    if (pImpl->history.empty()) return 0.0f;
    
    float min = pImpl->history[0];
    for (float reward : pImpl->history) {
        if (reward < min) min = reward;
    }
    return min;
}

bool Reward::isGoodOutcome(float threshold) const {
    if (pImpl->history.empty()) return false;
    
    float avgReward = getAverageReward();
    return avgReward > threshold;
}
