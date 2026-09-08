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
    // Real reward computation based on observation
    // Computes reward signals from sensory input, prediction errors, and environmental feedback
    
    // Get observation features
    std::vector<float> features = observation.getFeatures();
    
    // Initialize reward
    float reward = 0.0f;
    
    // Calculate goal achievement reward
    // Check if observation contains goal information
    for (float feature : features) {
        if (feature > 0.8f) {  // High positive feature indicates progress
            reward += feature * 0.1f;
        } else if (feature < -0.8f) {  // High negative feature indicates problem
            reward -= std::abs(feature) * 0.2f;
        }
    }
    
    // Incorporate prediction error if available
    // Positive prediction error (surprise) gives moderate reward
    float predictionError = observation.getPredictionError();
    if (!std::isnan(predictionError)) {
        reward += std::abs(predictionError) * 0.05f * (predictionError > 0.0f ? 1.0f : -0.5f);
    }
    
    // Time-dependent reward decay
    reward *= std::exp(-static_cast<float>(pImpl->history.size()) * 0.01f);
    
    // Store in history for learning
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
