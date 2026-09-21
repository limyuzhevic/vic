#include "Reward.hpp"
#include "../environment/Observation.hpp"
#include <algorithm>
#include <cmath>

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
    pImpl->history.push_back(pImpl->currentValue);
    // Keep history size manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Compute reward based on observation contents
    const auto& data = observation.getData();
    
    if (data.empty()) {
        return 0.0f;
    }
    
    // Reward positive values, penalize negative values
    // Apply decaying weight to older observations for temporal credit assignment
    float totalReward = 0.0f;
    float decayFactor = 0.95f;
    
    // Compute weighted sum of observation values
    for (size_t i = 0; i < data.size(); ++i) {
        // Apply exponential decay based on position
        float weight = std::pow(decayFactor, static_cast<float>(data.size() - i - 1));
        totalReward += data[i] * weight;
    }
    
    // Normalize based on observation variance
    float variance = 0.0f;
    float mean = totalReward / data.size();
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    // Add bonus for consistency (low variance)
    float consistencyBonus = 1.0f / (1.0f + variance);
    
    // Combine base reward with consistency bonus
    return totalReward * consistencyBonus;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
