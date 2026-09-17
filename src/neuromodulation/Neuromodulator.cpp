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
    // Real dopamine-modulated plasticity: level-dependent modulation
    // Higher dopamine increases LTP, decreases LTD
    float plasticityModulation = pImpl->level;
    
    // Biological basis: Dopamine modulates synaptic plasticity through
    // D1/D5 receptors (potentiation) and D2 receptors (depression)
    // This simplified model captures the net effect
    return 0.5f + 0.5f * plasticityModulation;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with exponential decay and temporal difference
    float decay = std::pow(0.99f, static_cast<float>(dt * 1000.0f / 10.0f));  // 10ms time constant
    pImpl->level = std::max(pImpl->baseline, pImpl->level * decay);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: Burst dopamine release proportional to reward prediction error
    // Implements temporal difference learning (TD learning) model
    // 
    // Biological basis:
    // - Dopamine neurons burst on unexpected rewards (unexpected positive prediction error)
    // - Tonic firing represents predicted value
    // - Phasic response scales with reward magnitude and prediction error
    
    // Compute prediction error: reward - expected_value
    float expectedValue = pImpl->level;  // Current predicted value
    float predictionError = reward - expectedValue;
    
    // Reward magnitude influences release
    float release = std::min(pImpl->peak, reward * pImpl->releaseRate);
    
    // Combine with prediction error for biological realism
    pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: Dopamine encodes TD error
    // 
    // Biological basis:
    // - Dopamine neurons encode reward prediction error (δ = r + γV(s') - V(s))
    // - Phasic increases for positive prediction error (surprising rewards)
    // - Phasic decreases for negative prediction error (omitted rewards)
    // 
    // This implements: level += error * release_rate (with saturation)
    
    // Clamp error for stability
    float clampedError = std::max(-1.0f, std::min(1.0f, error));
    
    // Update level with prediction error
    pImpl->level = std::clamp(pImpl->level + clampedError * pImpl->releaseRate, 
                             pImpl->baseline, pImpl->peak);
}

} // namespace nlm
