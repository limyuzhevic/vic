#include "Neuromodulator.hpp"
#include "../core/ErrorHandling/ErrorHierarchy.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    try {
        // Initialize with error handling
        setLevel(0.0f);  // Start at baseline
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize Dopamine neuromodulator: " + std::string(e.what()));
        // Continue with default initialization
    }
}

Dopamine::~Dopamine() {
    delete pImpl;
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    try {
        return std::clamp(pImpl->level, 0.0f, 1.0f);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting dopamine level: " + std::string(e.what()));
        return 0.0f;
    }
}

void Dopamine::setLevel(float level) {
    try {
        if (std::isnan(level) || std::isinf(level)) {
            throw NLMNeuromodulationError("Invalid dopamine level: " + std::to_string(level),
                                          NLMNeuromodulationError::ErrorType::DopamineError,
                                          "Dopamine::setLevel");
        }
        
        // Clamp to valid range [0.0, 1.0]
        pImpl->level = std::clamp(level, 0.0f, 1.0f);
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in setLevel: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error setting dopamine level: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to set dopamine level",
                                      NLMNeuromodulationError::ErrorType::DopamineError,
                                      "Dopamine::setLevel");
    }
}

float Dopamine::getPlasticityFactor() const {
    try {
        // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
        // PLACEHOLDER: Higher dopamine increases plasticity
        return 0.5f + 0.5f * pImpl->level;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting dopamine plasticity factor: " + std::string(e.what()));
        return 0.5f;  // Return neutral factor on error
    }
}

void Dopamine::update(TimestepDuration dt) {
    try {
        // Validate timestep
        if (dt <= 0.0f) {
            NLM_LOG_WARNING("Invalid timestep for dopamine update: " + std::to_string(dt));
            return;
        }
        
        // TODO PHASE 2: Implement real dopamine dynamics
        // PLACEHOLDER: Decay towards baseline
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dt);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error updating dopamine: " + std::string(e.what()));
    }
}

void Dopamine::signalReward(float reward) {
    try {
        // Validate reward value
        if (std::isnan(reward) || std::isinf(reward)) {
            throw NLMNeuromodulationError("Invalid reward value: " + std::to_string(reward),
                                          NLMNeuromodulationError::ErrorType::RewardError,
                                          "Dopamine::signalReward");
        }
        
        // TODO PHASE 2: Implement real reward signaling
        // PLACEHOLDER: Burst of dopamine on reward
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in signalReward: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error signaling reward: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to signal reward",
                                      NLMNeuromodulationError::ErrorType::RewardError,
                                      "Dopamine::signalReward");
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    try {
        // Validate prediction error
        if (std::isnan(error) || std::isinf(error)) {
            throw NLMNeuromodulationError("Invalid prediction error: " + std::to_string(error),
                                          NLMNeuromodulationError::ErrorType::PredictionError,
                                          "Dopamine::signalRewardPredictionError");
        }
        
        // TODO PHASE 2: Implement reward prediction error signaling
        // PLACEHOLDER: Dopamine responds to prediction error
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in signalRewardPredictionError: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error signaling reward prediction error: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to signal reward prediction error",
                                      NLMNeuromodulationError::ErrorType::PredictionError,
                                      "Dopamine::signalRewardPredictionError");
    }
}

// Helper function to convert NeuromodulatorType to string
inline const char* neuromodulatorTypeToString(NeuromodulatorType type) {
    static const char* typeStrings[] = {
        "Dopamine", "Curiosity", "Novelty", "PredictionError", "Reward", "Acetylcholine",
        "Norepinephrine", "Serotonin", "GABA", "Glutamate"
    };
    return typeStrings[static_cast<int>(type)];
}

} // namespace nlm

