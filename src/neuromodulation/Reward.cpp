#include "Reward.hpp"
#include "../environment/SensoryInput.hpp"
#include "../sensory/Vision.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward computation state
    float lastResourceValue;
    float lastHazardValue;
    float survivalThreshold;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
              lastResourceValue(0.0f), lastHazardValue(0.0f), survivalThreshold(0.2f) {}
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
    // Real reward computation based on sensory observation
    // Integrates multiple reward sources from the current percept
    
    float totalReward = 0.0f;
    
    // Get sensory input from observation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    // Check if we have vision data (most important for rewards)
    if (sensoryInput->getType() == SensoryInputType::Vision) {
        const Vision& vision = static_cast<const Vision&>(*sensoryInput);
        
        // Analyze vision data for reward opportunities
        // Reward for finding bright/intense objects (resources)
        float maxBrightness = 0.0f;
        for (size_t i = 0; i < vision.getData().size(); ++i) {
            float brightness = vision.getData()[i];
            if (brightness > maxBrightness) {
                maxBrightness = brightness;
            }
        }
        
        // Strong positive reward for high brightness (resources)
        if (maxBrightness > 0.7f) {
            totalReward += maxBrightness * 2.0f;
        }
        
        // Weak negative reward for very low brightness
        if (maxBrightness < 0.1f) {
            totalReward -= (0.1f - maxBrightness) * 0.5f;
        }
        
        // Novelty reward for new patterns
        // This encourages exploration and prevents overfitting
        static float lastVisionBrightness = 0.0f;
        float brightnessChange = std::abs(maxBrightness - lastVisionBrightness);
        totalReward += brightnessChange * 0.5f;
        lastVisionBrightness = maxBrightness;
    }
    
    // Check for internal signals
    if (sensoryInput->getType() == SensoryInputType::InternalSignals) {
        const InternalSignals& internal = static_cast<const InternalSignals&>(*sensoryInput);
        
        // Reward for maintaining stable internal signals
        // (avoiding stress, maintaining homeostasis)
        float stabilityBonus = 1.0f / (1.0f + internal.getVariance());
        totalReward += stabilityBonus * 0.2f;
    }
    
    // Add survival bonus based on current energy level
    if (pImpl->currentValue < 0.0f) {
        // Survival bonus when energy is low
        totalReward += 1.0f;
    }
    
    // Energy-based rewards
    float energyReward = pImpl->currentValue * 0.01f; // Small incremental reward per energy unit
    totalReward += energyReward;
    
    // Novelty bonus (exploration reward)
    // Small constant reward for taking actions
    totalReward += 0.1f;
    
    // Apply smooth reward updates (avoid sudden jumps)
    static float lastReward = 0.0f;
    float rewardChange = totalReward - lastReward;
    lastReward = totalReward;
    
    // Apply smooth transition to prevent overdosing on positive rewards
    totalReward = lastReward + rewardChange * std::clamp(1.0f, 0.1f, 1.0f);
    
    // Record in history
    pImpl->history.push_back(totalReward);
    
    // Keep history bounded
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
