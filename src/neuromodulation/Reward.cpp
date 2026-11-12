#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float lastPredictionError;  // Track for learning progress reward
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), lastPredictionError(0.0f) {}
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
    // Real reward computation from observation data
    // Extracts meaningful signals from SensoryInput to compute environmental rewards
    // Based on available sensory modalities (vision, touch, internal signals)
    
    float reward = 0.0f;
    
    if (!observation.isValid()) {
        return reward;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return reward;
    }
    
    // Compute reward based on available sensory data
    // This is a simplified reward function using available sensor modalities
    
    // 1. Resource detection via vision: bright spots in vision field suggest resources
    if (sensoryInput->getType() == SensoryInputType::Vision) {
        const auto& visionData = sensoryInput->getData();
        float maxVision = 0.0f;
        for (float value : visionData) {
            maxVision = std::max(maxVision, value);
        }
        
        // High vision values (bright spots) may indicate resources
        if (maxVision > 0.7f) {
            reward += maxVision * 2.0f;  // Reward for finding bright resources
        }
    }
    
    // 2. Touch/proximity signals: strong touch may indicate collision with hazards or resources
    if (sensoryInput->getType() == SensoryInputType::Touch) {
        const auto& touchData = sensoryInput->getData();
        float maxTouch = 0.0f;
        for (float value : touchData) {
            maxTouch = std::max(maxTouch, value);
        }
        
        // Strong touch could indicate hazard collision (negative reward)
        if (maxTouch > 0.5f) {
            reward -= maxTouch * 5.0f;  // Penalty for strong contact
        } else {
            reward += maxTouch * 0.5f;  // Small reward for mild contact
        }
    }
    
    // 3. Internal signals: energy/homeostatic regulation
    if (sensoryInput->getType() == SensoryInputType::InternalSignals) {
        const auto& internalData = sensoryInput->getData();
        // Look for energy-related signals (typically first few values)
        if (internalData.size() > 0) {
            float energy = internalData[0];
            // Penalize low energy levels
            if (energy < 0.3f) {
                reward -= (0.3f - energy) * 3.0f;
            }
        }
    }
    
    // 4. Learning progress: based on prediction error from available data
    // This is a simplified version - in real implementation would use actual prediction error
    float learningSignal = 0.0f;
    if (sensoryInput->getType() == SensoryInputType::Vision) {
        const auto& visionData = sensoryInput->getData();
        // Calculate novelty/diversity in visual input as proxy for learning progress
        float meanVision = 0.0f;
        for (float value : visionData) meanVision += value;
        meanVision /= visionData.size();
        
        float variance = 0.0f;
        for (float value : visionData) {
            variance += (value - meanVision) * (value - meanVision);
        }
        variance /= visionData.size();
        learningSignal = std::sqrt(variance);  // Standard deviation as learning signal
    }
    
    // Reward for learning progress (encourages exploration)
    if (learningSignal > 0.0f) {
        reward += learningSignal * 0.5f;
    }
    
    // Store for next computation
    pImpl->lastPredictionError = learningSignal;
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
