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
    // Phase 6: Compute reward from observation with real logic
    
    // Extract energy from observation (e.g., internal energy signal)
    float energy = 0.0f;
    if (!observation.getInternal().empty()) {
        energy = observation.getInternal()[0];  // Energy level (0-100)
    }
    
    // Extract health from observation (e.g., internal health signal)
    float health = 0.0f;
    if (!observation.getInternal().empty() && observation.getInternal().size() > 1) {
        health = observation.getInternal()[1];  // Health level (0-1)
    }
    
    // Extract sensory signals for environment interaction
    float resources = 0.0f;  // For food/resource gathering
    float hazards = 0.0f;   // For hazard avoidance
    float novel = 0.0f;     // For novelty seeking
    
    // Process vision for resource/hazard detection
    const auto& vision = observation.getVision();
    for (float pixel : vision) {
        // Normalize pixel from [0,1] to [0,100]
        float intensity = pixel * 100.0f;
        
        // Bright pixels might indicate resources or hazards
        if (intensity > 70.0f) {
            // Likely resource or hazard
            if (health < 50.0f && intensity > 80.0f) {
                // Bright area when low health = likely hazard
                hazards += (intensity - 80.0f) * 2.0f;
            } else {
                // Bright area when healthy = likely resource
                resources += intensity;
            }
        }
        
        // Medium intensity = novelty/potential interest
        if (intensity > 30.0f && intensity < 70.0f) {
            novel += intensity * 0.5f;
        }
    }
    
    // Calculate reward based on state and sensory input
    // Higher energy = positive reward (survival)
    float survivalReward = energy * 0.5f;
    
    // Resource gathering = positive reward
    float resourceReward = resources * 2.0f;
    
    // Hazard avoidance = negative reward (penalty)
    float hazardPenalty = hazards * 3.0f;
    
    // Novelty seeking = small positive reward (curiosity incentive)
    float noveltyReward = novel * 0.1f;
    
    // Combine rewards with weighting
    float totalReward = survivalReward + resourceReward - hazardPenalty + noveltyReward;
    
    // Add complexity: health affects sensitivity to resources/hazards
    float healthFactor = health / 100.0f;  // Normalize to [0,1]
    
    // When health is low, resources are more valuable
    totalReward += resources * healthFactor * 1.5f;
    
    // When health is high, we can take more risks for novelty
    float riskTolerance = healthFactor;
    totalReward += novel * riskTolerance * 0.3f;
    
    // Clamp to reasonable range
    totalReward = std::clamp(totalReward, -10.0f, 10.0f);
    
    // Update history for reinforcement learning analysis
    pImpl->history.push_back(totalReward);
    if (pImpl->history.size() > 100) {  // Keep history manageable
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
