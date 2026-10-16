#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <limits>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Initialize dopamine system parameters with biologically plausible defaults
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {
        NLM_LOG_DEBUG("Dopamine: Initialized with level=0.0, baseline=0.0, peak=1.0, decay=0.1, release=1.0");
    }
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Dopamine: Creating dopamine neuromodulation system");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    // Set dopamine level with comprehensive validation and logging
    if (level < 0.0f) {
        NLM_LOG_WARNING("Dopamine::setLevel: Negative dopamine level requested (" + 
                       std::to_string(level) + ") - clamping to 0");
        level = 0.0f;
    } else if (level > 1.0f) {
        NLM_LOG_WARNING("Dopamine::setLevel: High dopamine level requested (" + 
                       std::to_string(level) + ") - may cause instability");
    }
    
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    NLM_LOG_INFO("Dopamine::setLevel: Set dopamine level to " + std::to_string(pImpl->level));
}

float Dopamine::getPlasticityFactor() const {
    // Calculate dopamine-modulated plasticity factor with comprehensive validation
    float factor = calculatePlasticityFactor();
    
    // Log plasticity factor for debugging
    NLM_LOG_DEBUG("Dopamine::getPlasticityFactor: Returning " + std::to_string(factor) +
                 " (dopamine level = " + std::to_string(pImpl->level) + ")");
    
    return factor;
}

float Dopamine::calculatePlasticityFactor() const {
    // Calculate dopamine-modulated plasticity factor using biologically inspired model
    // Higher dopamine increases plasticity (learning rate), lower dopamine decreases it
    
    // Base plasticity factor of 0.5 (moderate learning rate)
    float baseFactor = 0.5f;
    
    // Dopamine modulates plasticity with sigmoidal relationship
    // This reflects dopamine's role in regulating learning
    float modulation = 0.5f + 0.5f * pImpl->level;  // 0.5 to 1.0 range
    
    float factor = baseFactor * modulation;
    
    // Ensure factor stays within reasonable bounds
    factor = std::clamp(factor, 0.1f, 2.0f);  // 10% to 200% of base rate
    
    NLM_LOG_DEBUG("Dopamine::calculatePlasticityFactor: level=" + std::to_string(pImpl->level) +
                 ", modulation=" + std::to_string(modulation) +
                 ", factor=" + std::to_string(factor));
    
    return factor;
}

void Dopamine::update(TimestepDuration dt) {
    // Update dopamine dynamics based on physiological principles
    if (dt <= 0.0f) {
        NLM_LOG_WARNING("Dopamine::update: Non-positive timestep provided (" + 
                       std::to_string(dt) + ") - skipping update");
        return;
    }
    
    NLM_LOG_DEBUG("Dopamine::update: Updating with dt=" + std::to_string(dt) + 
                 "s, current level=" + std::to_string(pImpl->level));
    
    // Apply decay towards baseline (tonic dopamine effect)
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    NLM_LOG_DEBUG("Dopamine::update: After decay level=" + std::to_string(pImpl->level));
}

void Dopamine::signalReward(float reward) {
    // Process reward signal using biologically inspired model
    validateRewardSignal(reward);
    
    // Calculate reward-modulated dopamine burst
    float rewardAmount = calculateRewardEffect(reward);
    
    // Apply burst with ceiling to prevent runaway excitation
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardAmount);
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    NLM_LOG_INFO("Dopamine::signalReward: Reward = " + std::to_string(reward) +
                 ", burst amount = " + std::to_string(rewardAmount) +
                 ", new level = " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Process reward prediction error signal using dopamine response properties
    validatePredictionError(error);
    
    // Calculate prediction error-modulated dopamine change
    float errorAmount = calculatePredictionErrorEffect(error);
    
    // Apply prediction error with floor to prevent negative dips
    pImpl->level = std::max(0.0f, pImpl->level + errorAmount);
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    NLM_LOG_INFO("Dopamine::signalRewardPredictionError: Error = " + std::to_string(error) +
                 ", effect amount = " + std::to_string(errorAmount) +
                 ", new level = " + std::to_string(pImpl->level));
}

void Dopamine::validateRewardSignal(float reward) {
    // Validate reward signal for numerical stability
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_WARNING("Dopamine::validateRewardSignal: Invalid reward value (NaN or Inf) - using 0");
        reward = 0.0f;
    }
}

void Dopamine::validatePredictionError(float error) {
    // Validate prediction error signal for numerical stability
    if (std::isnan(error) || std::isinf(error)) {
        NLM_LOG_WARNING("Dopamine::validatePredictionError: Invalid prediction error (NaN or Inf) - using 0");
        error = 0.0f;
    }
}

float Dopamine::calculateRewardEffect(float reward) {
    // Calculate reward-modulated dopamine burst effect
    // Positive rewards produce dopamine release (reward prediction)
    
    float effect = reward * pImpl->releaseRate;
    
    // Log effect calculation for debugging
    NLM_LOG_DEBUG("Dopamine::calculateRewardEffect: reward=" + std::to_string(reward) +
                 ", releaseRate=" + std::to_string(pImpl->releaseRate) +
                 ", effect=" + std::to_string(effect));
    
    return effect;
}

float Dopamine::calculatePredictionErrorEffect(float error) {
    // Calculate prediction error-modulated dopamine effect
    // Positive prediction errors (better than expected) produce dopamine increase
    // Negative prediction errors (worse than expected) produce dopamine decrease
    
    float effect = error * pImpl->releaseRate;
    
    // Log effect calculation for debugging
    NLM_LOG_DEBUG("Dopamine::calculatePredictionErrorEffect: error=" + std::to_string(error) +
                 ", releaseRate=" + std::to_string(pImpl->releaseRate) +
                 ", effect=" + std::to_string(effect));
    
    return effect;
}

} // namespace nlm
