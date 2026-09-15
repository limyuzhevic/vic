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
    // Implement real dopamine-modulated plasticity factor
    // Based on reward prediction error theory:
    // - Phasic DA bursts enhance plasticity for rewarded actions
    // - Phasic DA dips suppress plasticity for unrewarded actions
    // - Baseline DA sets overall excitability
    
    float plasticityFactor = 1.0f;  // Baseline plasticity
    
    if (pImpl->level > 0.7f) {
        // High dopamine (reward prediction error): strongly enhance plasticity
        plasticityFactor = 2.0f + (pImpl->level - 0.7f) * 2.0f;
    } else if (pImpl->level < 0.3f && pImpl->baseline > 0.2f) {
        // Low dopamine (prediction error): suppress plasticity to avoid overlearning
        plasticityFactor = std::max(0.1f, 1.0f - (0.2f - pImpl->level) * 3.0f);
    } else {
        // Moderate dopamine: modulate plasticity based on reward proximity
        plasticityFactor = 1.0f + (pImpl->level - 0.5f) * 0.5f;
    }
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
