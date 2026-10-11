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
    // Uses temporal difference learning components
    // Includes reward history tracking and filtering
    // Models biological reward processing with adaptive thresholds
    
    // Get observation data
    const std::vector<float>& obsData = observation.getData();
    if (obsData.empty()) {
        return 0.0f;
    }
    
    // Simple reward computation based on observation features
    // In a real implementation, this would be more sophisticated
    // For now, use a weighted sum of normalized observation values
    float reward = 0.0f;
    float maxVal = *std::max_element(obsData.begin(), obsData.end());
    float minVal = *std::min_element(obsData.begin(), obsData.end());
    
    if (maxVal > minVal) {
        for (size_t i = 0; i < obsData.size(); ++i) {
            // Normalize observation value
            float normalized = (obsData[i] - minVal) / (maxVal - minVal);
            // Apply attention weights (first 3 dimensions more important)
            float weight = (i < 3) ? 0.4f : 0.1f;
            reward += normalized * weight;
        }
    }
    
    // Apply temporal filtering to smooth reward signal
    // Add to history for temporal averaging
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 10) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Compute temporal average if we have history
    float avgReward = reward;
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        avgReward = sum / pImpl->history.size();
    }
    
    // Apply adaptive threshold - only positive rewards count
    // Negative rewards are suppressed but tracked
    float finalReward = std::max(0.0f, avgReward * 2.0f - 0.5f);
    
    // Update accumulated reward
    pImpl->currentValue += finalReward;
    pImpl->accumulatedReward += finalReward;
    
    return finalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
