#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHierarchy.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {
    try {
        // Initialize with error handling
        reset();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize Reward neuromodulator: " + std::string(e.what()));
        // Continue with default initialization
    }
}

Reward::~Reward() {
    delete pImpl;
}

float Reward::getValue() const {
    try {
        return pImpl->currentValue;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting reward value: " + std::string(e.what()));
        return 0.0f;
    }
}

void Reward::setValue(float value) {
    try {
        if (std::isnan(value) || std::isinf(value)) {
            throw NLMNeuromodulationError("Invalid reward value: " + std::to_string(value),
                                          NLMNeuromodulationError::ErrorType::RewardError,
                                          "Reward::setValue");
        }
        
        pImpl->currentValue = value;
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in setValue: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error setting reward value: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to set reward value",
                                      NLMNeuromodulationError::ErrorType::RewardError,
                                      "Reward::setValue");
    }
}

void Reward::add(float delta) {
    try {
        if (std::isnan(delta) || std::isinf(delta)) {
            throw NLMNeuromodulationError("Invalid reward delta: " + std::to_string(delta),
                                          NLMNeuromodulationError::ErrorType::RewardError,
                                          "Reward::add");
        }
        
        pImpl->accumulatedReward += delta;
        pImpl->currentValue += delta;
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in add: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error adding to reward: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to add to reward",
                                      NLMNeuromodulationError::ErrorType::RewardError,
                                      "Reward::add");
    }
}

void Reward::reset() {
    try {
        pImpl->currentValue = 0.0f;
        pImpl->accumulatedReward = 0.0f;
        NLM_LOG_INFO("Reward system reset");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error resetting reward: " + std::string(e.what()));
    }
}

float Reward::computeReward(const Observation& observation) const {
    try {
        // TODO PHASE 2: Implement real reward computation from observation
        // PLACEHOLDER: Returns 0
        return 0.0f;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error computing reward: " + std::string(e.what()));
        return 0.0f;
    }
}

const std::vector<float>& Reward::getHistory() const {
    try {
        return pImpl->history;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting reward history: " + std::string(e.what()));
        static const std::vector<float> emptyVector;
        return emptyVector;
    }
}

void Reward::clearHistory() {
    try {
        pImpl->history.clear();
        NLM_LOG_INFO("Reward history cleared");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error clearing reward history: " + std::string(e.what()));
    }
}

} // namespace nlm