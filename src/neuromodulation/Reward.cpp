#include "Reward.hpp"
#include <algorithm>
#include <cmath>

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
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0 - will be implemented with observation-based reward
    // For now, just return 0 to maintain existing behavior
    return 0.0f;
}

float Reward::computeObservingReward(const Observation& observation) const {
    // Enhanced reward computation based on sensory observations
    float reward = 0.0f;
    
    if (!observation.isValid()) {
        return reward;
    }
    
    const SensoryInput* sensory = observation.getSensoryInput();
    if (!sensory) {
        return reward;
    }
    
    // Get sensory data
    const std::vector<float>& data = sensory->getData();
    if (data.empty()) {
        return reward;
    }
    
    // Reward computation based on observation content
    // This is a simple implementation that can be expanded
    
    // 1. Vision-based rewards (resources vs hazards) - Selective
    // Vision neurons typically encode objects as brightness/values
    // - Bright pixels (>0.8) could indicate resources
    // - Dark pixels (<0.3) could indicate hazards or walls
    // - Medium values (0.3-0.8) could indicate empty space or neutral objects
    float maxVision = 0.0f;
    float minVision = 1.0f;
    float sumVision = 0.0f;
    int visionCount = 0;
    
    for (float val : data) {
        maxVision = std::max(maxVision, val);
        minVision = std::min(minVision, val);
        sumVision += val;
        visionCount++;
    }
    
    // Calculate contrast to detect distinct objects
    float contrast = std::max(maxVision - minVision, 0.0f);
    
    // Resource detection (bright objects) - selective
    if (maxVision > 0.8f) {
        float resourceValue = (maxVision - 0.8f) * 15.0f;  // Scale to 0-15 range
        reward += resourceValue;
        
        // Bonus for high contrast (distinct objects are more valuable)
        if (contrast > 0.5f) {
            reward += 2.0f;
        }
    }
    
    // Hazard detection (dark objects in vision) - selective
    if (minVision < 0.25f && maxVision > 0.15f && contrast > 0.3f) {
        float hazardValue = (0.25f - minVision) * 8.0f;  // Scale to 0-8 range
        reward -= hazardValue;
        
        // Hazard objects have higher contrast
        if (contrast > 0.6f) {
            reward -= 3.0f;  // Extra penalty for clear hazards
        }
    }
    
    // 2. Selective novelty detection (encourage exploration of new patterns)
    // If observation is too similar to previous, reduce reward significantly
    float noveltyFactor = calculateNoveltyFactor();
    reward *= noveltyFactor;
    
    // 3. Predictive reward (encourage predictive accuracy) - selective
    // This would integrate with prediction system
    float predictionError = calculatePredictionError();
    reward += predictionError * 0.2f;  // Smaller weight for prediction error
    
    // 4. Intrinsic motivation (curiosity, exploration) - selective
    // Strong curiosity for patterns that are partially familiar but not fully predicted
    float curiositySignal = calculateCuriositySignal(data);
    reward += curiositySignal * 0.05f;
    
    // 5. Developmental stage adjustment - selective
    // Different developmental stages should learn differently
    float plasticityMod = getDevelopmentalPlasticity();
    reward *= plasticityMod;
    
    // 6. Memory-based reward modulation
    // Give bonus for learning new patterns vs repeating old ones
    float memoryBonus = calculateMemoryBonus(data);
    reward += memoryBonus;
    
    // 7. Action outcome-based reward (from world simulation)
    // This would integrate with world reward system
    // float worldReward = getWorldActionReward();
    // reward += worldReward;
    
    // Add to accumulated reward with selective weighting
    if (std::abs(reward) > 0.001f) {
        // Scale reward based on learning stage
        float stageFactor = getLearningStageFactor();
        float scaledReward = reward * stageFactor;
        add(scaledReward);
    }
    
    // Record observation for selective learning
    recordObservation(observation);
    
    return reward;
}

float Reward::calculateNoveltyFactor() const {
    // Novelty factor: 1.0 for novel observations, decreases for familiar ones
    // This encourages exploration of new patterns while avoiding saturation
    
    if (pImpl->history.empty()) {
        return 1.0f;  // First observation is always novel
    }
    
    // Calculate average reward level
    float avgReward = 0.0f;
    for (float r : pImpl->history) {
        avgReward += r;
    }
    avgReward /= static_cast<float>(pImpl->history.size());
    
    // Novelty factor: decrease when average reward is high (familiar patterns)
    float novelty = std::max(0.2f, 1.0f - std::min(avgReward * 0.1f, 0.8f));
    return novelty;
}

float Reward::calculatePredictionError() const {
    // Calculate prediction error based on observed vs expected patterns
    // This would integrate with prediction system in a full implementation
    
    // Placeholder: return small random value to encourage exploration
    // In real implementation, this would use prediction system
    return 0.0f;
}

float Reward::calculateCuriositySignal(const std::vector<float>& data) const {
    // Calculate curiosity signal based on sensory variation and prediction error
    
    // Calculate sensory variation (entropy-like measure)
    float mean = 0.0f;
    for (float val : data) {
        mean += val;
    }
    mean /= static_cast<float>(data.size());
    
    float variance = 0.0f;
    for (float val : data) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= static_cast<float>(data.size());
    
    float variation = std::sqrt(variance);
    
    // Curiosity is high when variation is moderate (not too predictable, not too random)
    float curiosity = std::max(0.0f, 1.0f - std::abs(variation - 0.3f) * 2.0f);
    
    return curiosity;
}

float Reward::calculateMemoryBonus(const std::vector<float>& data) const {
    // Calculate bonus for learning new patterns vs repeating old ones
    
    // Simple implementation: give bonus for patterns that are not in recent history
    // In real implementation, this would use memory systems
    
    // For now, return small positive bonus to encourage learning
    return 0.1f;
}

float Reward::getLearningStageFactor() const {
    // Get learning stage factor based on accumulated reward
    // Early stages have higher learning rates
    
    float rewardLevel = std::abs(pImpl->currentValue);
    
    if (rewardLevel < 5.0f) {
        return 1.5f;  // High learning in early stages
    } else if (rewardLevel < 20.0f) {
        return 1.0f;  // Normal learning
    } else {
        return 0.5f;  // Reduced learning to avoid saturation
    }
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
