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
    // Real reward computation based on observation features
    // For now, implement a simple heuristic: higher value for denser regions
    if (!observation.gridData.empty()) {
        float totalValue = 0.0f;
        for (float cell : observation.gridData) {
            totalValue += cell;
        }
        
        float avgValue = totalValue / observation.gridData.size();
        
        // Reward based on density and value distribution
        float reward = avgValue * (1.0f + std::min(1.0f, totalValue / 100.0f));
        
        // Add stochastic component
        float noise = (rand() % 200 - 100) / 1000.0f; // ±0.1 noise
        reward = std::max(0.0f, reward + noise);
        
        return reward;
    }
    
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
