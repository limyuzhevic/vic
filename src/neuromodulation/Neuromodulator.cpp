// Neuromodulator implementations for Phase 6 integrated brain
//
// This file contains implementations of all neuromodulation systems:
// - Dopamine: Reward and reinforcement learning signal
// - Acetylcholine: Attention and memory consolidation (placeholder)
// - Norepinephrine: Arousal and vigilance (placeholder)
// - Serotonin: Mood, impulsivity, and social behavior (placeholder)
//
// Neuromodulators are crucial for:
// - Dynamic adjustment of neural excitability
// - Gating learning processes based on behavioral relevance
// - Maintaining alertness and focus
// - Regulating emotional states and decision making

#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Dopamine implementation with proper error handling
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate Dopamine::Impl");
    }
}

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
    // PHASE 2 IMPROVEMENT: Implement dopamine-modulated plasticity factor
    // This factor dynamically adjusts synaptic plasticity based on dopamine levels
    // representing the temporal difference error in reward prediction
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 2: Implement dopamine dynamics including baseline
    // Dopaminergic neurons maintain a baseline activity that decays toward baseline
    // representing the tonic dopamine signal
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // PHASE 2: Implement reward signaling with burst dynamics
    // Phasic dopamine responses produce transient bursts proportional to reward prediction error
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 2: Implement reward prediction error signaling
    // Dopamine responses to prediction errors in the reward prediction
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
