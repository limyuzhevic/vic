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
    if (!observation.isValid()) {
        return 0.0f;  // Invalid observation yields no reward
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;  // No sensory input yields no reward
    }
    
    // Get the sensory data
    const std::vector<float>& data = sensoryInput->getData();
    
    // Compute reward based on sensory input type and data
    float reward = 0.0f;
    
    if (strcmp(sensoryInput->getType(), "Vision") == 0) {
        // Vision reward: positive for bright pixels, negative for dark pixels
        // This encourages the agent to notice and respond to visual features
        for (float pixelValue : data) {
            // Normalize pixel values (typically 0-255) to -1 to 1 range
            float normalized = (pixelValue / 255.0f) * 2.0f - 1.0f;
            reward += normalized * 0.01f;  // Small contribution per pixel
        }
        // Cap reward to reasonable range
        reward = std::max(-1.0f, std::min(1.0f, reward));
        
    } else if (strcmp(sensoryInput->getType(), "Audio") == 0) {
        // Audio reward: positive for high amplitude, negative for low amplitude
        // Encourages detection of sound events
        float maxAmplitude = *std::max_element(data.begin(), data.end(),
                                               [](float a, float b) { return std::abs(a) < std::abs(b); });
        reward = maxAmplitude * 0.1f;  // Scale amplitude to reward
        
    } else if (strcmp(sensoryInput->getType(), "InternalSignals") == 0) {
        // Internal signals reward: encourage stable firing patterns
        // Look for activity patterns that suggest learning or exploration
        float sumOfSquares = 0.0f;
        for (float value : data) {
            sumOfSquares += value * value;
        }
        float variance = sumOfSquares / data.size() - std::pow(std::accumulate(data.begin(), data.end(), 0.0f) / data.size(), 2);
        
        // Positive reward for moderate variance (indicates active processing)
        // Negative reward for too stable or too chaotic patterns
        if (variance > 0.01f && variance < 0.5f) {
            reward = 0.5f;  // Moderate activity is good
        } else if (variance >= 0.5f) {
            reward = -0.2f;  // Too chaotic, penalize
        } else {
            reward = -0.1f;  // Too stable, slight penalty
        }
    }
    
    // Store reward in history for learning
    if (pImpl->history.size() >= 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    pImpl->history.push_back(reward);
    
    // Apply dopamine baseline modulation
    float baselineReward = reward + (pImpl->currentValue * 0.5f); // Dopamine affects reward sensitivity
    
    return baselineReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
