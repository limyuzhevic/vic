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
    // Phase 2: Implement real dopamine-modulated plasticity factor
    // Dopamine affects plasticity based on its level and history
    // High dopamine (reward prediction error) increases plasticity
    // Low or negative dopamine decreases plasticity (learning suppression)
    
    // Biological basis:
    // - Dopamine modulates synaptic strength via D1/D5 receptors (enhancing) and D2/D3 receptors (inhibiting)
    // - Neural adaptation: dopamine response changes with learning
    // - Baseline tonic dopamine maintains background level
    
    float baseFactor = 0.5f;  // Baseline plasticity at 50%
    float modulation = pImpl->level;  // Direct linear relationship
    
    // Saturation at high levels to prevent runaway potentiation
    if (pImpl->level > 0.8f) {
        modulation = 0.8f + (pImpl->level - 0.8f) * 0.5f;
    }
    
    return baseFactor + modulation * 0.6f;  // Range: 0.5 to 1.1
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 2: Implement real dopamine dynamics
    // Multi-timescale dynamics: fast phasic signals + slow tonic modulation
    // Homeostatic regulation to maintain optimal dopamine levels
    // Interaction with prediction error signals
    
    // Decay towards baseline (tonic level)
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Implement temporal dynamics for more realistic dopamine signaling
    // Add slow integrating component for baseline maintenance
    static double slowAdaptation = 0.0;
    slowAdaptation += (pImpl->baseline - pImpl->level) * dt * 0.01;
    pImpl->level += slowAdaptation * 0.01;
}

void Dopamine::signalReward(float reward) {
    // Phase 2: Implement real reward signaling
    // Phasic dopamine bursts for unexpected rewards (reward prediction error > 0)
    // Phasic dips for omitted rewards (prediction error < 0)
    // Duration and intensity based on reward magnitude and salience
    
    // Calculate reward prediction error contribution
    float rewardImpact = std::min(1.0f, reward * pImpl->releaseRate);
    
    // Burst response: add to current level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardImpact);
    
    // Track reward history for potential adaptation
    pImpl->baseline = (pImpl->baseline * 0.9f) + (reward * 0.1f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 2: Implement reward prediction error signaling
    // Dopaminergic neurons encode prediction errors: bursts for better-than-expected rewards
    // dips for worse-than-expected rewards
    // Modulates learning rates accordingly
    
    // Prediction error signal modulates dopamine level
    float errorImpact = error * pImpl->releaseRate;
    
    // Apply with bounds to prevent runaway excitation
    if (error > 0.0f) {
        // Positive prediction error: burst response
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorImpact);
    } else {
        // Negative prediction error: dip response
        pImpl->level = std::max(0.0f, pImpl->level + errorImpact * 0.5f);
    }
    
    // Error influences baseline adaptation over longer timescales
    pImpl->baseline += error * 0.01f;
}

} // namespace nlm
