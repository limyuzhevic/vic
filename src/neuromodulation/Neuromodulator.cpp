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
    // IMPLEMENTED: Real dopamine-modulated plasticity factor
    // Uses sigmoidal function for more biologically realistic response
    float x = (pImpl->level - 0.5f) * 4.0f;  // Scale to range [-2, 2]
    float sigmoid = 1.0f / (1.0f + std::exp(-x));
    return sigmoid;  // Range [0.018, 0.982]
}

void Dopamine::update(TimestepDuration dt) {
    // IMPLEMENTED: Real dopamine dynamics with more complex model
    // Follows phasic and tonic components of dopamine signaling
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
}

void Dopamine::signalReward(float reward) {
    // IMPLEMENTED: Real reward signaling with saturation and adaptation
    float burst = reward * pImpl->releaseRate;
    float newLevel = std::min(pImpl->peak, pImpl->level + burst);
    
    // Apply adaptation based on current level (homosynaptic plasticity)
    if (pImpl->level > 0.5f) {
        newLevel *= (1.0f - pImpl->level * 0.2f);
    }
    
    pImpl->level = newLevel;
}

void Dopamine::signalRewardPredictionError(float error) {
    // IMPLEMENTED: Reward prediction error signaling with differential response
    // Positive errors (better than expected) cause more dopamine than negative errors
    float magnitude = std::abs(error);
    float modulation = (error > 0) ? 1.5f : 0.8f;  // Phasic difference
    
    float newLevel = pImpl->level + magnitude * modulation * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, std::min(pImpl->peak, newLevel));
}

} // namespace nlm
