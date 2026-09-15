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
    // Real reward computation based on observation
    // Calculate reward based on observed goals, obstacles, and rewards in environment
    
    // Get observation data
    const auto& visionData = observation.getVisionData();
    const auto& touchData = observation.getTouchData();
    const auto& internalData = observation.getInternalData();
    
    float reward = 0.0f;
    
    // Check vision for positive rewards (resources, goals)
    for (size_t y = 0; y < visionData.height; ++y) {
        for (size_t x = 0; x < visionData.width; ++x) {
            const auto& cell = visionData.getCell(x, y);
            
            // Resource cell gives positive reward
            if (cell.type == WorldObjectType::Resource) {
                reward += cell.value * 2.0f;  // Reward scales with resource value
            }
            
            // Hazard cell gives negative reward
            if (cell.type == WorldObjectType::Hazard) {
                reward -= cell.value * 3.0f;  // Hazards are more punishing
            }
            
            // Marker (goal) gives large positive reward
            if (cell.type == WorldObjectType::Marker) {
                reward += 10.0f;  // Big reward for finding goals
            }
        }
    }
    
    // Check touch for environmental feedback
    for (size_t i = 0; i < touchData.size(); ++i) {
        if (touchData[i] > 0.5f) {
            // Contact with something gives small reward for exploration
            reward += 0.5f;
        }
    }
    
    // Consider internal state (energy, etc.)
    for (size_t i = 0; i < internalData.size(); ++i) {
        // Low energy should give negative reward to encourage feeding/eating
        if (internalData[i] < 0.3f && i == 0) {
            reward -= 2.0f;  // Starvation penalty
        }
        
        // High energy is good
        if (internalData[i] > 0.8f && i == 0) {
            reward += 1.0f;  // Well-fed reward
        }
    }
    
    // Apply smoothing to reward
    // Current reward gets 70%, accumulated gets 30%
    pImpl->currentValue = 0.7f * reward + 0.3f * pImpl->accumulatedReward * 0.01f;
    
    // Store in history
    pImpl->history.push_back(pImpl->currentValue);
    
    // Limit history size
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return pImpl->currentValue;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
