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
    // Phase 2: Implement real reward computation from observation
    // Multi-factor reward computation based on biological principles:
    // - Primary rewards: basic needs (food, water, shelter)
    // - Secondary rewards: learned associations and social rewards
    // - Hedonic modulation: pleasure/pain signaling
    // - Integration with prediction error for adaptive learning
    
    // For now, use a simple heuristic based on observation data
    const auto& vision = observation.getVision();
    const auto& touch = observation.getTouch();
    const auto& internal = observation.getInternal();
    
    float reward = 0.0f;
    
    // Vision-based rewards (finding food, avoiding obstacles, social signals)
    // Bright colors = potential food or social interaction
    for (size_t i = 0; i < vision.size(); ++i) {
        if (vision[i] > 0.7f) {  // High brightness
            reward += (vision[i] - 0.7f) * 2.0f;  // Scale up to 0.6 reward per bright pixel
        }
    }
    
    // Touch-based rewards (collision, temperature, physical contact)
    for (size_t i = 0; i < touch.size(); ++i) {
        if (touch[i] > 0.5f) {  // Collision detected
            reward += 0.1f;  // Small reward for interaction
        }
    }
    
    // Internal state rewards (energy levels, homeostasis)
    for (size_t i = 0; i < internal.size(); ++i) {
        // Normalize internal signals to [0,1] and convert to reward
        float normalized = (internal[i] + 1.0f) * 0.5f;  // Assume range [-1,1]
        if (normalized > 0.8f) {  // Good internal state
            reward += (normalized - 0.8f) * 0.5f;
        } else if (normalized < 0.2f) {  // Poor internal state
            reward -= (0.2f - normalized) * 0.3f;  // Negative reward
        }
    }
    
    // Apply biological constraints and limits
    reward = std::clamp<float>(reward, -1.0f, 1.0f);
    
    // Add some noise for biological realism (very small)
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    float noise = (rng() % 200 - 100) / 100.0f * 0.01f;  // ±0.01 noise
    reward += noise;
    
    // Store in history for analysis
    pImpl->history.push_back(reward);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
