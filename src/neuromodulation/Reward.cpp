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
    // Extract sensory input from observation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const auto& signals = sensoryInput->getAllSignals();
    
    // Compute reward based on sensory signals
    float reward = 0.0f;
    
    // Vision-based reward: resources (bright) are good, hazards (dark) are bad
    // Based on vision signal interpretation:
    // - Values > 0.6 indicate resource proximity
    // - Values < 0.4 indicate hazard proximity
    // - Values around 0.5 indicate neutral terrain
    
    float visionSignal = 0.0f;
    size_t visionCount = 0;
    
    // Find vision signal in the sensory input
    // Vision signals typically come from vision neurons with high indices
    for (size_t i = 0; i < signals.size(); ++i) {
        // Check if this signal is from a vision neuron (vision neurons have high indices)
        if (i >= signals.size() * 0.7) {  // Top 30% of signals are typically vision
            visionSignal += signals[i];
            visionCount++;
        }
    }
    
    if (visionCount > 0) {
        visionSignal /= static_cast<float>(visionCount);
        
        // Resource detection: bright vision (high values) indicate proximity to resources
        if (visionSignal > 0.6f) {
            reward += (visionSignal - 0.6f) * 20.0f;  // Scale to 0-20 reward
        }
        // Hazard detection: dark vision (low values) indicate proximity to hazards
        else if (visionSignal < 0.4f) {
            reward -= (0.4f - visionSignal) * 20.0f;  // Scale to 0-20 penalty
        }
    }
    
    // Touch-based reward: contact with objects provides rewards/penalties
    float touchSignal = 0.0f;
    size_t touchCount = 0;
    
    // Find touch signal in the sensory input
    // Touch signals typically come from touch neurons with specific indices
    for (size_t i = 0; i < signals.size(); ++i) {
        // Check if this signal is from a touch neuron (touch neurons have indices in the 40-60% range)
        if (i >= signals.size() * 0.4 && i < signals.size() * 0.6) {
            touchSignal += signals[i];
            touchCount++;
        }
    }
    
    if (touchCount > 0) {
        touchSignal /= static_cast<float>(touchCount);
        
        // Touch reward: based on object type detection
        // Higher touch signals indicate resource contact (positive reward)
        // Lower touch signals indicate hazard contact (negative reward)
        if (touchSignal > 0.7f) {
            reward += (touchSignal - 0.7f) * 30.0f;  // Resource contact
        } else if (touchSignal < 0.3f) {
            reward -= (0.3f - touchSignal) * 30.0f;  // Hazard contact
        } else {
            // Moderate touch signals indicate neutral contact
            reward += (touchSignal - 0.5f) * 5.0f;  // Small neutral reward
        }
    }
    
    // Internal state reward: maintain energy and health
    // Based on internal signals if available
    float energySignal = 0.0f;
    size_t energyCount = 0;
    
    for (size_t i = 0; i < signals.size(); ++i) {
        // Internal signals typically come from internal neurons (indices in the 20-40% range)
        if (i >= signals.size() * 0.2 && i < signals.size() * 0.4) {
            energySignal += signals[i];
            energyCount++;
        }
    }
    
    if (energyCount > 0) {
        energySignal /= static_cast<float>(energyCount);
        
        // Energy management reward: maintain good energy levels
        // Higher internal signals indicate good energy state
        if (energySignal > 0.7f) {
            reward += (energySignal - 0.7f) * 10.0f;  // Good energy state
        } else if (energySignal < 0.3f) {
            reward -= (0.3f - energySignal) * 10.0f;  // Poor energy state
        }
    }
    
    // Apply temporal discounting and exploration bonus
    // Future rewards are less valuable than immediate rewards
    float temporalDiscount = 1.0f;
    
    // Exploration bonus: reward for visiting new locations (based on novelty)
    // This encourages exploration and discovery
    static std::vector<float> visitedLocations;
    // Note: In a real implementation, you would track visited locations
    // For this implementation, we'll just add a small exploration bonus
    reward += 0.1f;  // Small exploration bonus
    
    // Clamp reward to reasonable range
    reward = std::clamp(reward, -50.0f, 50.0f);
    
    // Record the computed reward in history
    pImpl->history.push_back(reward);
    // Keep history size manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    NLM_LOG_DEBUG("Computed reward: " + std::to_string(reward) + 
                  " (vision: " + std::to_string(visionSignal) +
                  ", touch: " + std::to_string(touchSignal) +
                  ", energy: " + std::to_string(energySignal) + ")");
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
