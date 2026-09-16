#include "Neuromodulator.hpp"
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    /*
     * Enhanced reward signaling with adaptive learning
     * 
     * Implements a dual-process model of reward:
     * 1. Phasic response: immediate burst for unexpected reward
     * 2. Tonic response: baseline level modulated by reward history
     * 
     * Mathematical formulation:
     * level(t+1) = max(level(t) - decay, min(peak, level(t) + reward * release))
     * 
     * Biological basis:
     * - VTA/SNc dopamine neurons show phasic bursts for reward prediction errors
     * - Tonic firing represents baseline motivation
     * - Sustained elevation encodes reward magnitude
     * - Integration with memory systems for prediction
     * 
     * Neural dynamics:
     * - Short time constant (100-500ms) for phasic responses
     * - Longer time constant (seconds) for tonic modulation
     * - Spike probability increases with dopamine level
     * - Downstream targets have dopamine concentration-dependent effects
     */
    
    // Calculate reward prediction error based on baseline
    float predictionError = reward - pImpl->baseline;
    
    // Phasic response (rapid burst for positive prediction error)
    if (predictionError > 0.0f) {
        float phasicBurst = predictionError * pImpl->releaseRate * 1.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + phasicBurst);
    }
    
    // Tonic modulation (sustained increase for repeated rewards)
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * 0.5f * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    /*
     * Enhanced reward prediction error signaling
     * 
     * Implements Sutton-Barto RSSPE (Rescorla-Wagner) with temporal difference learning:
     * 
     * delta = reward + gamma * V(s') - V(s)
     * 
     * Where:
     *   reward = actual received reward
     *   V(s') = predicted reward at next state
     *   V(s) = predicted reward at current state
     *   gamma = discount factor (0.0-1.0)
     * 
     * Biological implementation:
     * - Positive error: burst firing (reward > expected)
     * - Negative error: pause firing (reward < expected)
     * - Response magnitude scales with error magnitude
     * - Temporal integration across multiple time scales
     * 
     * Neural circuitry:
     * - Ventral tegmental area (VTA) dopamine neurons
     * - Nucleus accumbens projections
     * - Prefrontal cortex integration
     * - Hippocampal prediction signals
     */
    
    // Apply temporal difference learning rule
    float learningRate = 0.01f + 0.02f * pImpl->level;  // Adaptive learning rate
    float temporalError = error * learningRate;
    
    // Positive error: potentiation of dopamine response
    if (error > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + temporalError);
        // Enhance prediction for similar future events
        pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + temporalError * 0.3f);
    }
    
    // Negative error: depression of dopamine response
    else if (error < 0.0f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + temporalError);
        // Adjust prediction downward for similar future events
        pImpl->baseline = std::max(pImpl->baseline, pImpl->baseline + temporalError * 0.5f);
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

} // namespace nlm
