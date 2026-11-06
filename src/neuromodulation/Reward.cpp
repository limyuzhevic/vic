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
    // IMPLEMENTED: Calculate reward based on observation data
    // Based on SimpleWorld::applyMotorCommand and interact():
    // - Positive reward for resource consumption (+30.0f)
    // - Negative reward for hazard hits (-15.0f)
    // - Small cost for movement and turning
    // - Zero reward for neutral actions (Wait, Look, successful movement)
    
    const auto* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const auto& data = sensoryInput->getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Simple reward heuristic based on sensory data
    // This is a basic implementation that can be enhanced
    float maxReward = -10.0f;  // Default penalty for bad states
    float bestReward = 0.0f;   // Default neutral
    
    // Check if we have vision data (first few channels might represent proximity)
    // This is a placeholder heuristic that can be improved
    if (data.size() >= 4) {
        // Basic proximity-based reward: higher if agent is near center/positive areas
        // This is a simple example - real implementation should use domain knowledge
        float proximity = data[0];  // Use first channel as example proximity metric
        
        // Reward proximity to beneficial areas
        if (proximity > 0.7f) {
            maxReward = 10.0f;  // Good proximity
        } else if (proximity > 0.4f) {
            maxReward = 5.0f;   // Moderate proximity
        }
        
        // Apply neuromodulation factors
        float neuromodulationFactor = pImpl->currentValue;  // Current neuromodulation level
        if (neuromodulationFactor > 0.0f) {
            // Dopamine enhances reward signals
            maxReward *= (1.0f + neuromodulationFactor);
        } else {
            // Negative modulation reduces reward effectiveness
            maxReward *= (1.0f + neuromodulationFactor * 0.5f);
        }
    }
    
    // Incorporate novelty and curiosity factors
    // Novelty detection could influence exploration rewards
    float noveltyFactor = 1.0f;  // Placeholder - could be enhanced with actual novelty detection
    if (noveltyFactor > 1.0f) {
        // Novel states get exploration bonus
        maxReward += 2.0f;
    }
    
    // Clamp to reasonable range
    if (maxReward < -50.0f) maxReward = -50.0f;
    if (maxReward > 50.0f) maxReward = 50.0f;
    
    // Add to history for learning
    pImpl->history.push_back(maxReward);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    NLM_LOG_DEBUG("Reward computed: " + std::to_string(maxReward) + 
                  " (neuromodulation: " + std::to_string(pImpl->currentValue) + ")");
    
    return maxReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
