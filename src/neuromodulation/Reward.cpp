#include "Reward.hpp"
#include "../environment/Environment.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward computation parameters
    float resourceReward;
    float hazardReward;
    float noveltyReward;
    float energyReward;
    float diversityReward;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f),
             resourceReward(10.0f), hazardReward(-15.0f),
             noveltyReward(2.0f), energyReward(1.0f),
             diversityReward(0.5f) {}
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
    pImpl->history.push_back(delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    // Get sensory data
    const auto& vision = sensoryInput->getVision();
    const auto& touch = sensoryInput->getTouch();
    const auto& internal = sensoryInput->getInternal();
    const auto& proprioception = sensoryInput->getProprioception();
    
    // Initialize reward components
    float totalReward = 0.0f;
    float resourceReward = 0.0f;
    float hazardReward = 0.0f;
    float noveltyReward = 0.0f;
    float energyReward = 0.0f;
    float diversityReward = 0.0f;
    
    // === Resource Detection ===
    // Positive reward for finding bright objects (resources)
    float maxVision = 0.0f;
    for (float v : vision) {
        maxVision = std::max(maxVision, v);
    }
    
    if (maxVision > 0.7f) {
        resourceReward += pImpl->resourceReward * maxVision;
    }
    
    // === Hazard Detection ===
    // Negative reward for dark objects (hazards)
    float minVision = 1.0f;
    for (float v : vision) {
        minVision = std::min(minVision, v);
    }
    
    if (minVision < 0.3f) {
        hazardReward += pImpl->hazardReward * (1.0f - minVision);
    }
    
    // === Touch-Based Rewards ===
    // Positive reward for contact with resources
    float maxTouch = 0.0f;
    for (float t : touch) {
        maxTouch = std::max(maxTouch, t);
    }
    
    if (maxTouch > 0.5f) {
        resourceReward += pImpl->resourceReward * maxTouch * 0.5f;
    }
    
    // === Energy Reward ===
    // Small penalty for excessive energy use (based on internal signals)
    float totalInternal = 0.0f;
    for (float i : internal) {
        totalInternal += i;
    }
    
    if (totalInternal > 2.0f) {
        energyReward = -pImpl->energyReward * (totalInternal - 2.0f) * 0.1f;
    }
    
    // === Novelty Detection ===
    // Reward for novel sensory patterns
    noveltyReward = pImpl->noveltyReward * (1.0f - maxVision) * (1.0f - minVision);
    
    // === Diversity Reward ===
    // Reward for exploring different areas (based on proprioception)
    float proprioceptionVariation = 0.0f;
    for (float p : proprioception) {
        proprioceptionVariation += std::abs(p - 0.5f);
    }
    
    diversityReward = pImpl->diversityReward * proprioceptionVariation * 0.5f;
    
    // Combine all reward components
    totalReward = resourceReward + hazardReward + noveltyReward + energyReward + diversityReward;
    
    // Clamp to reasonable range
    totalReward = std::clamp(totalReward, -20.0f, 20.0f);
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm

