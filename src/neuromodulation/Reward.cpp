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
    // Implement real reward computation from observation
    // Based on agent's internal state and environmental feedback
    
    float totalReward = 0.0f;
    
    // Reward from internal signals (energy, health, etc.)
    const auto& internal = observation.getInternal();
    if (internal.size() >= 1) {
        // Positive reward for high energy (maintaining homeostasis)
        totalReward += internal[0] * 2.0f;  // energy / maxEnergy * 2
    }
    if (internal.size() >= 2) {
        // Reward for good health (avoiding hazards)
        totalReward += internal[1] * 1.5f;  // health value
    }
    
    // Reward from movement (energy cost)
    if (internal.size() >= 3) {
        // Negative reward for movement (energy expenditure)
        totalReward -= internal[2] * 0.5f;  // -0.5 per unit movement
    }
    
    // Reward from thinking state
    if (internal.size() >= 4) {
        // Reward for active thinking (cognitive processing)
        totalReward += internal[3] * 0.3f;  // +0.3 per unit thinking activity
    }
    
    // Vision-based reward (approximating what the agent should value)
    const auto& vision = observation.getVision();
    if (!vision.empty()) {
        float visionMean = 0.0f;
        for (float v : vision) visionMean += v;
        visionMean /= vision.size();
        
        // Moderate brightness: resource-like patterns are bright, hazards are dark
        if (visionMean > 0.6f) {
            // Bright scene: potentially resource-rich, small positive reward
            totalReward += 0.1f;
        } else if (visionMean < 0.3f) {
            // Dark scene: potentially hazardous, small negative reward
            totalReward -= 0.1f;
        }
        
        // Reward for visual variety (interesting scenes)
        float stdDev = 0.0f;
        for (float v : vision) {
            stdDev += (v - visionMean) * (v - visionMean);
        }
        stdDev = std::sqrt(stdDev / vision.size());
        totalReward += stdDev * 0.5f;  // More variety = more interesting = reward
    }
    
    // Touch-based reward (proximity to objects)
    const auto& touch = observation.getTouch();
    if (!touch.empty()) {
        float maxTouch = 0.0f;
        for (float t : touch) maxTouch = std::max(maxTouch, t);
        
        // Strong touch (close to objects) could indicate resource or hazard
        // Without knowing type, this is ambiguous - we need environmental context
        // For now, small neutral reward for proximity awareness
        totalReward += maxTouch * 0.2f;
    }
    
    // Proprioception-based reward (body state awareness)
    const auto& proprio = observation.getProprioception();
    if (!proprio.empty()) {
        // Reward for well-balanced body state
        float balanceScore = 0.0f;
        for (size_t i = 0; i < std::min<size_t>(3, proprio.size()); ++i) {
            // Positions and orientations should be in reasonable ranges
            if (i == 0 || i == 1) {  // Position: should be in [0,1]
                balanceScore += std::max(0.0f, 1.0f - std::abs(proprio[i] - 0.5f) * 2.0f);
            } else if (i == 2) {  // Orientation: should be near 0 or 1
                balanceScore += std::max(0.0f, 1.0f - std::abs((proprio[i] - 0.0f) * 2.0f - 1.0f));
            }
        }
        balanceScore /= 3.0f;
        totalReward += balanceScore * 0.3f;
    }
    
    // Learning-based reward: reward for acquiring new concepts
    if (pImpl->history.size() > 0) {
        // Decay old rewards gradually
        float decay = 0.95f;
        for (auto& hist : pImpl->history) hist *= decay;
    }
    
    // Clip reward to reasonable bounds
    totalReward = std::clamp(totalReward, -2.0f, 5.0f);
    
    // Store in history for learning
    pImpl->history.push_back(totalReward);
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
