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
    if (!observation.isValid() || !observation.getSensoryInput()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    const std::vector<float>& data = sensoryInput->getData();
    
    // Compute basic reward based on sensory input
    float reward = 0.0f;
    
    // For Vision input, reward based on object detection and proximity
    if (std::string(sensoryInput->getType()) == "Vision") {
        // Vision data analysis for reward
        // Look for positive patterns (bright objects = resources)
        // and negative patterns (dark objects = hazards)
        float sum = std::accumulate(data.begin(), data.end(), 0.0f);
        float mean = data.empty() ? 0.0f : sum / data.size();
        
        // Higher brightness typically indicates resources
        if (mean > 0.7f) {
            reward = 10.0f; // Resource found
        } else if (mean < 0.3f) {
            reward = -5.0f; // Hazard detected
        } else {
            reward = 1.0f; // Neutral exploration reward
        }
    }
    
    // For Internal Signals, use learned preferences
    else if (std::string(sensoryInput->getType()) == "InternalSignals") {
        // Internal signals from brain state
        if (data.size() >= 2) {
            // Higher activation might indicate approaching goal
            float activation = data[0];
            if (activation > 0.8f) {
                reward = 5.0f;
            } else if (activation < 0.2f) {
                reward = -2.0f;
            }
        }
    }
    
    // Add small exploration reward to encourage discovery
    reward += 0.01f;
    
    // Update history
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
