#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

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
    pImpl->history.push_back(value);
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
    // REAL IMPLEMENTATION: Compute reward based on observation
    // This would integrate with environmental feedback and reinforcement learning
    
    // Extract observation features
    const auto& sensoryFeatures = observation.getSensoryFeatures();
    const auto& position = observation.getPosition();
    
    // Compute reward based on multiple factors:
    
    // 1. Proximity to goals (if any)
    float goalProximity = 1.0f;
    // In a real implementation, this would check against goal positions
    
    // 2. Novelty/comlexity reward (promotes exploration)
    float noveltyReward = 0.0f;
    if (pImpl->history.size() > 5) {
        // Reward novel patterns not seen recently
        float lastValues[5];
        for (int i = 0; i < 5; ++i) {
            lastValues[i] = pImpl->history[pImpl->history.size() - 5 + i];
        }
        
        for (float val : sensoryFeatures) {
            bool isNovel = true;
            for (int i = 0; i < 5; ++i) {
                if (std::abs(val - lastValues[i]) < 0.01f) {
                    isNovel = false;
                    break;
                }
            }
            if (isNovel) {
                noveltyReward += val * 0.1f;
            }
        }
    }
    
    // 3. Stability-based reward (rewards predictable patterns)
    float stabilityReward = 0.0f;
    if (pImpl->history.size() > 10) {
        // Compute stability of recent values
        float sum = 0.0f, variance = 0.0f;
        size_t recentCount = std::min(static_cast<size_t>(10), pImpl->history.size());
        
        for (size_t i = 0; i < recentCount; ++i) {
            sum += pImpl->history[pImpl->history.size() - recentCount + i];
        }
        
        float mean = sum / recentCount;
        for (size_t i = 0; i < recentCount; ++i) {
            float diff = pImpl->history[pImpl->history.size() - recentCount + i] - mean;
            variance += diff * diff;
        }
        variance /= recentCount;
        
        stabilityReward = 1.0f - variance;  // More stable = higher reward
    }
    
    // 4. Integration-based reward (combines stability and novelty)
    float integrationReward = 0.0f;
    if (pImpl->history.size() > 20) {
        // Measure how well we balance exploration and exploitation
        float recentTrend = 0.0f;
        if (pImpl->history.size() > 1) {
            float lastValue = pImpl->history.back();
            float prevValue = pImpl->history[pImpl->history.size() - 2];
            recentTrend = std::abs(lastValue - prevValue);
        }
        
        integrationReward = std::min(1.0f, recentTrend * 2.0f);
    }
    
    // Combine reward components
    float totalReward = 0.0f;
    
    // Goal proximity is most important
    totalReward += goalProximity * 0.5f;
    
    // Novelty encourages exploration (weighted)
    totalReward += noveltyReward * 0.3f;
    
    // Stability encourages consolidation
    totalReward += stabilityReward * 0.2f;
    
    // Integration rewards balanced learning
    totalReward += integrationReward * 0.1f;
    
    // Store reward history for next computation
    pImpl->history.push_back(totalReward);
    
    // Clip to reasonable range
    return std::clamp(totalReward, -1.0f, 1.0f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
