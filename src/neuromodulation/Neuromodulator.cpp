#include "Neuromodulator.hpp"
#include "ErrorHandling/Error.hpp"
#include "ErrorHandlingSystem.hpp"
#include "Validation/ValidationFramework.hpp"
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

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    try {
        // Validate input using error handling system
        if (std::isnan(level) || std::isinf(level)) {
            ErrorHandlingSystem::getInstance().logError(
                "Invalid dopamine level: " + std::to_string(level), 
                ErrorCategory::Neuromodulation, 
                ErrorSeverity::Error,
                {{ "parameter", "level" },
                 { "value", std::to_string(level) },
                 { "valid_range", "[0.0, 1.0]" }}
            );
            throw DopamineError("Invalid dopamine level: " + std::to_string(level));
        }
        
        pImpl->level = std::clamp(level, 0.0f, 1.0f);
    } catch (const NLMError& e) {
        throw;
    } catch (const std::exception& e) {
        ErrorHandlingSystem::getInstance().logError(
            "Error setting dopamine level: " + std::string(e.what()),
            ErrorCategory::Neuromodulation,
            ErrorSeverity::Error,
            {{ "parameter", "level" },
             { "value", std::to_string(level) },
             { "action", "setLevel" }}
        );
        throw DopamineError("Error setting dopamine level: " + std::string(e.what()));
    }
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    try {
        // Validate timestep using error handling system
        if (dt <= 0.0) {
            ErrorHandlingSystem::getInstance().logError(
                "Invalid timestep for dopamine update: " + std::to_string(dt),
                ErrorCategory::Neuromodulation,
                ErrorSeverity::Warning,
                {{ "parameter", "dt" },
                 { "value", std::to_string(dt) },
                 { "valid_range", "> 0" }}
            );
            return;  // Skip update with invalid timestep
        }
        
        // TODO PHASE 2: Implement real dopamine dynamics
        // PLACEHOLDER: Decay towards baseline
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } catch (const NLMError& e) {
        throw;
    } catch (const std::exception& e) {
        ErrorHandlingSystem::getInstance().logError(
            "Error updating dopamine: " + std::string(e.what()),
            ErrorCategory::Neuromodulation,
            ErrorSeverity::Error,
            {{ "parameter", "dt" },
             { "value", std::to_string(dt) },
             { "action", "update" }}
        );
    }
}

void Dopamine::signalReward(float reward) {
    try {
        // Validate reward value using error handling system
        if (std::isnan(reward) || std::isinf(reward)) {
            ErrorHandlingSystem::getInstance().logError(
                "Invalid reward value: " + std::to_string(reward),
                ErrorCategory::Neuromodulation,
                ErrorSeverity::Error,
                {{ "parameter", "reward" },
                 { "value", std::to_string(reward) },
                 { "valid_range", "finite number" }}
            );
            throw NeuromodulationError("Invalid reward value: " + std::to_string(reward));
        }
        
        // TODO PHASE 2: Implement real reward signaling
        // PLACEHOLDER: Burst of dopamine on reward
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    } catch (const NLMError& e) {
        throw;
    } catch (const std::exception& e) {
        ErrorHandlingSystem::getInstance().logError(
            "Error signaling reward: " + std::string(e.what()),
            ErrorCategory::Neuromodulation,
            ErrorSeverity::Error,
            {{ "parameter", "reward" },
             { "value", std::to_string(reward) },
             { "action", "signalReward" }}
        );
        throw NeuromodulationError("Error signaling reward: " + std::string(e.what()));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    try {
        // TODO PHASE 2: Implement reward prediction error signaling
        // PLACEHOLDER: Dopamine responds to prediction error
        
        // Validate prediction error using error handling system
        if (std::isnan(error) || std::isinf(error)) {
            ErrorHandlingSystem::getInstance().logError(
                "Invalid reward prediction error: " + std::to_string(error),
                ErrorCategory::Neuromodulation,
                ErrorSeverity::Warning,
                {{ "parameter", "error" },
                 { "value", std::to_string(error) },
                 { "valid_range", "finite number" }}
            );
            return;  // Skip signal with invalid error
        }
        
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    } catch (const NLMError& e) {
        throw;
    } catch (const std::exception& e) {
        ErrorHandlingSystem::getInstance().logError(
            "Error signaling reward prediction error: " + std::string(e.what()),
            ErrorCategory::Neuromodulation,
            ErrorSeverity::Warning,
            {{ "parameter", "error" },
             { "value", std::to_string(error) },
             { "action", "signalRewardPredictionError" }}
        );
        // Don't throw for prediction errors, just log and skip
    }
}

} // namespace nlm
