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
    try {
        // Validate value
        if (std::isnan(value) || std::isinf(value)) {
            Logger::getGlobal().error("Reward::setValue: Invalid reward value: " + std::to_string(value));
            return;
        }
        
        pImpl->currentValue = value;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Reward::setValue: Exception while setting reward value: " + std::string(e.what()));
    }
}

void Reward::add(float delta) {
    try {
        // Validate delta
        if (std::isnan(delta) || std::isinf(delta)) {
            Logger::getGlobal().error("Reward::add: Invalid delta value: " + std::to_string(delta));
            return;
        }
        
        pImpl->accumulatedReward += delta;
        pImpl->currentValue += delta;
        
        // Record in history with bounds checking
        if (pImpl->history.size() >= 10000) {
            Logger::getGlobal().warning("Reward::add: Reward history at capacity, oldest entries will be lost");
            pImpl->history.erase(pImpl->history.begin());
        }
        
        pImpl->history.push_back(delta);
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("Reward::add: Memory allocation failed");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Reward::add: Exception while adding reward: " + std::string(e.what()));
    }
}

void Reward::reset() {
    try {
        pImpl->currentValue = 0.0f;
        pImpl->accumulatedReward = 0.0f;
        pImpl->history.clear();
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Reward::reset: Exception while resetting reward: " + std::string(e.what()));
    }
}

float Reward::computeReward(const Observation& observation) const {
    try {
        // Validate observation
        if (!observation.getSensoryInput()) {
            Logger::getGlobal().warning("Reward::computeReward: Observation has no sensory input");
            return 0.0f;
        }
        
        // TODO PHASE 2: Implement real reward computation from observation
        // PLACEHOLDER: Returns 0
        return 0.0f;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Reward::computeReward: Exception while computing reward: " + std::string(e.what()));
        return 0.0f;
    }
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    try {
        pImpl->history.clear();
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Reward::clearHistory: Exception while clearing history: " + std::string(e.what()));
    }
}

} // namespace nlm
