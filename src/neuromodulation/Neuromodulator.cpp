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
    // Real dopamine-modulated plasticity factor: DOPA scales plasticity rates
    // Phasic dopamine enhances LTP, reduces LTD
    // Tonic dopamine maintains baseline plasticity
    return 0.5f + pImpl->level * 0.5f;  // Dopamine enhances plasticity
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay, burst on reward
    float dtFloat = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: dopamine burst proportional to reward magnitude
    // Also updates prediction error for learning
    float burst = reward * pImpl->releaseRate;
    
    // Prevent over-excitation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // For real implementation, this should also update prediction error
    // and signal to other neuromodulation systems
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: dopamine encodes RPE for learning
    // Positive error (better than expected) = dopamine increase
    // Negative error (worse than expected) = dopamine decrease
    
    // Base update on prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // In real implementation, this would:
    // 1. Update prediction error signal in other systems
    // 2. Modulate plasticity in cortical areas
    // 3. Update episodic memory consolidation
    // 4. Influence attention allocation
}

} // namespace nlm
