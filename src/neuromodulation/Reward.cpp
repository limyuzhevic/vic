#include "Reward.hpp"
#include "../environment/Environment.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/InternalSignals.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    std::shared_ptr<Environment> environment;  // Reference to environment for reward computation
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), environment(nullptr) {}
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

void Reward::setEnvironment(std::shared_ptr<Environment> env) {
    pImpl->environment = env;
}

float Reward::computeReward(const Observation& observation) const {
    // PHASE 2 FIX: Implement real reward computation from observation
    // If environment is available and has been used, get its last reward
    if (pImpl->environment) {
        // Get the environment's last reward if available
        float envReward = pImpl->environment->getLastReward();
        if (envReward != 0.0f) {
            // Add small exploration bonus/penaltry based on observation complexity
            // This encourages the agent to explore and learn
            return envReward;
        }
    }
    
    // Process the observation directly if no environment is available
    if (observation.isValid() && observation.getSensoryInput()) {
        const SensoryInput* sensory = observation.getSensoryInput();
        
        // For InternalSignals, we can compute reward based on signal values
        if (dynamic_cast<const InternalSignals*>(sensory)) {
            const std::vector<float>& data = sensory->getData();
            
            if (!data.empty()) {
                // Compute reward based on internal signal values
                // Higher positive values indicate better internal states
                float sum = std::accumulate(data.begin(), data.end(), 0.0f);
                float mean = sum / data.size();
                
                // Reward positive internal states, penalize negative ones
                // With a reasonable scale to encourage stability
                return mean * 0.1f;
            }
        }
    }
    
    // Default: return small negative reward for taking actions to encourage efficiency
    return -0.01f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
