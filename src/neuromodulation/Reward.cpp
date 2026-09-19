#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    static constexpr size_t MAX_HISTORY_SIZE = 1000;
    
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
    if (!observation.isValid()) {
        NLM_LOG_WARNING("Invalid observation provided for reward computation");
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        NLM_LOG_WARNING("Observation has no sensory input");
        return 0.0f;
    }
    
    // Basic reward computation based on sensory input
    const std::vector<float>& data = sensoryInput->getData();
    
    // Initialize reward based on sensory modality
    float reward = 0.0f;
    
    // Extract features from data
    float mean = 0.0f;
    float variance = 0.0f;
    size_t count = data.size();
    
    if (count > 0) {
        // Compute basic statistics from sensory data
        for (float val : data) {
            mean += val;
        }
        mean /= static_cast<float>(count);
        
        // Compute variance (measure of change/novelty)
        for (float val : data) {
            variance += (val - mean) * (val - mean);
        }
        variance /= static_cast<float>(count);
        
        // Vision-specific processing
        if (std::string(sensoryInput->getType()) == "Vision") {
            // For vision, check contrast (variance indicates interesting patterns)
            float contrast = std::sqrt(variance);
            reward = contrast * 10.0f;  // Scale contrast to reasonable reward range
            
            // Additional vision features: check for edge-like patterns
            // Simple edge detection by finding sharp transitions
            for (size_t i = 1; i < count; ++i) {
                if (std::abs(data[i] - data[i-1]) > 0.1f) {
                    reward += 0.01f;  // Small bonus for edges
                }
            }
        } 
        // Audio-specific processing
        else if (std::string(sensoryInput->getType()) == "Audio") {
            // For audio, reward based on signal strength and variability
            // Higher variance could indicate interesting sounds
            float audioEnergy = std::sqrt(variance);
            reward = audioEnergy * 5.0f;
            
            // Check for sudden changes (like speech or important events)
            for (size_t i = 1; i < count; ++i) {
                if (std::abs(data[i] - data[i-1]) > 0.2f) {
                    reward += 0.02f;  // Sudden changes get bonus
                }
            }
        } 
        // Internal signals (brain state) processing
        else if (std::string(sensoryInput->getType()) == "InternalSignals") {
            // For internal signals, reward based on signal patterns
            // Higher positive signals indicate good internal state
            for (float val : data) {
                if (val > 0.0f) {
                    reward += val * 0.1f;  // Positive internal states reward
                } else if (val < -0.5f) {
                    reward -= 0.1f;  // Strong negative signals penalize
                }
            }
        } 
        // Generic processing for other sensory types
        else {
            // Generic reward based on signal magnitude and variance
            float magnitude = 0.0f;
            for (float val : data) {
                magnitude += std::abs(val);
            }
            reward = magnitude / static_cast<float>(count);
            
            // Bonus for higher variance (novelty)
            reward += variance * 2.0f;
        }
    }
    
    // Apply bounds and smoothing
    reward = std::max(0.0f, std::min(reward, 10.0f));  // Bound to reasonable range
    
    // Store in history with capacity check
    if (pImpl->history.size() < pImpl->MAX_HISTORY_SIZE) {
        pImpl->history.push_back(reward);
    }
    
    NLM_LOG_DEBUG("Computed reward: %f from %s input with %zu samples", 
                  reward, sensoryInput->getType(), count);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

float Reward::getAverageReward(size_t window) const {
    if (pImpl->history.empty()) {
        return 0.0f;
    }
    
    size_t startPos = (pImpl->history.size() > window) ? 
                      pImpl->history.size() - window : 0;
    
    float sum = 0.0f;
    for (size_t i = startPos; i < pImpl->history.size(); ++i) {
        sum += pImpl->history[i];
    }
    
    return sum / static_cast<float>(pImpl->history.size() - startPos);
}

bool Reward::isPositive() const {
    return getValue() > 0.0f;
}

float Reward::getMagnitude() const {
    return std::abs(getValue());
}

} // namespace nlm
