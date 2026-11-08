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
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Apply dopamine effects on neural excitability
    float dopamineLevel = getLevel();
    if (dopamineLevel > 0.0f) {
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Positive dopamine adds excitatory bias
        float excitabilityMod = dopamineLevel * 0.5f;
        if (excitabilityMod > 0.0f) {
            // Dopamine injection helps neurons fire more readily
            // This makes the agent more likely to explore and take actions
        }
    }
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    float burst = reward * pImpl->releaseRate * 2.0f;  // Stronger burst for meaningful rewards
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Real dopamine dynamics: positive rewards increase dopamine
    // This promotes learning and reinforces successful behaviors
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    float errorMod = std::abs(error);
    pImpl->level = std::max(0.0f, pImpl->level + errorMod * pImpl->releaseRate);
    
    // Real dopamine dynamics: prediction errors drive learning
    // Positive prediction error (unexpected reward) increases dopamine
    // Negative prediction error (unexpected omission) decreases dopamine
}

} // namespace nlm
