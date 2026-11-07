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
    // Phase 6: Implement comprehensive reward computation based on actual environmental input
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* input = observation.getSensoryInput();
    if (!input) {
        return 0.0f;
    }
    
    // Multi-factor reward computation
    float totalReward = 0.0f;
    
    // 1. Resource collection reward (based on internal signals)
    if (input->getType() == "InternalSignals") {
        const std::vector<float>& data = input->getData();
        
        // Energy level (positive reward for maintaining energy)
        if (!data.empty()) {
            float energy = data[0];  // Energy is typically first signal
            if (energy > 0.7f) {
                totalReward += 0.1f;  // Small positive reward for good energy management
            } else if (energy < 0.3f) {
                totalReward -= 0.2f;  // Penalty for low energy
            }
        }
        
        // Health (positive factor)
        if (data.size() >= 2) {
            float health = data[1];
            totalReward += health * 0.05f;
        }
        
        // Movement state (reward for active exploration)
        if (data.size() >= 3 && data[2] > 0.5f) {
            totalReward += 0.05f;
        }
    }
    
    // 2. Novelty-based exploration reward
    // This is computed by the Novelty system and injected here
    if (pImpl->currentValue != 0.0f) {
        totalReward += pImpl->currentValue * 0.2f;  // Novelty contribution
    }
    
    // 3. Prediction error-based learning reward
    // This is computed by the PredictionError system and injected here
    if (pImpl->currentValue > 0.0f) {
        totalReward += pImpl->currentValue * 0.3f;  // Prediction error contribution
    }
    
    // 4. Curiosity-based exploration reward
    // This is computed by the Curiosity system and injected here
    if (pImpl->currentValue > 0.0f) {
        totalReward += pImpl->currentValue * 0.1f;  // Curiosity contribution
    }
    
    // 5. Base survival reward (small positive to encourage continued operation)
    totalReward += 0.01f;
    
    // 6. Stability bonus (reward for not rapidly changing state)
    if (pImpl->history.size() >= 5) {
        float recentVariation = 0.0f;
        size_t compareCount = std::min(5UL, pImpl->history.size());
        for (size_t i = 1; i < compareCount; ++i) {
            recentVariation += std::abs(pImpl->history[i] - pImpl->history[i-1]);
        }
        recentVariation /= compareCount;
        if (recentVariation < 0.1f) {
            totalReward += 0.05f;  // Reward for stable state
        }
    }
    
    // Add to accumulated reward and update history
    pImpl->currentValue = totalReward;
    pImpl->accumulatedReward += totalReward;
    pImpl->history.push_back(totalReward);
    
    // Keep history manageable
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
