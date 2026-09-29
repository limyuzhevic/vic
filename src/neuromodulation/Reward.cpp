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
    // Enhanced reward computation from observations
    // Uses observation data for more sophisticated reward signals
    
    // Extract features from observation data
    std::vector<float> features = extractFeaturesFromObservation(observation);
    
    // Compute reward based on feature values
    float reward = computeFeatureBasedReward(features);
    
    // Apply reward history smoothing
    reward = applyHistorySmoothing(reward);
    
    // Store in history
    pImpl->history.push_back(reward);
    
    // Maintain history size
    const size_t maxHistorySize = 1000;
    if (pImpl->history.size() > maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return reward;
}

// Helper method to extract features from observation
std::vector<float> Reward::extractFeaturesFromObservation(const Observation& observation) const {
    // Simple feature extraction from observation data
    std::vector<float> features = observation.getData();
    
    // If observation is empty, create default features
    if (features.empty()) {
        features = std::vector<float>(5, 0.0f); // Default feature vector
    }
    
    return features;
}

// Helper method to compute feature-based reward
float Reward::computeFeatureBasedReward(const std::vector<float>& features) const {
    // Compute reward as weighted sum of features
    float reward = 0.0f;
    
    // Simple reward computation based on feature statistics
    if (features.empty()) {
        return 0.0f;
    }
    
    // Compute mean feature value
    float sum = 0.0f;
    for (float value : features) {
        sum += value;
    }
    float mean = sum / features.size();
    
    // Compute variance
    float variance = 0.0f;
    for (float value : features) {
        float diff = value - mean;
        variance += diff * diff;
    }
    variance /= features.size();
    
    // Use mean as primary reward signal with variance as modulation
    reward = mean + 0.1f * (std::sqrt(variance) - 1.0f);
    
    // Apply nonlinear scaling to ensure reasonable reward bounds
    return std::clamp(reward, -1.0f, 1.0f);
}

// Helper method to apply history smoothing
float Reward::applyHistorySmoothing(float currentReward) const {
    if (pImpl->history.empty()) {
        return currentReward;
    }
    
    // Apply exponential moving average for smoothing
    float alpha = 0.3f; // Smoothing factor
    float smoothedReward = alpha * currentReward + (1.0f - alpha) * pImpl->history.back();
    
    return smoothedReward;
}

} // namespace nlm
