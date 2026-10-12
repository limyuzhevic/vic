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
    // PHASE 2: Real dopamine-modulated plasticity factor
    // Dopamine modulates plasticity via D1/D2 receptor dynamics
    // Base plasticity + dopamine-dependent modulation
    float basePlasticity = 0.5f;
    float dopamineModulation = pImpl->level * 0.5f;
    
    // Nonlinear response to dopamine levels
    float sigmoidalModulation = 1.0f / (1.0f + std::exp(-2.0f * (pImpl->level - 0.5f)));
    
    return basePlasticity + sigmoidalModulation * dopamineModulation;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // PHASE 2: Real reward signaling
    // Dopamine bursts proportionally to reward prediction error
    float phasicBurst = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicBurst);
    
    // Log the reward signal for debugging
    NLM_LOG_DEBUG("Dopamine reward signal: " + std::to_string(reward) + 
                  " (current level: " + std::to_string(pImpl->level) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 2: Reward prediction error signaling
    // Phasic dopamine responds to prediction errors (Rescorla-Wagner style)
    float rpeSignal = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, std::min(pImpl->peak, pImpl->level + rpeSignal));
    
    // Encode prediction error magnitude for learning modulation
    float predictionErrorInfluence = std::tanh(std::abs(error)) * pImpl->level;
    
    NLM_LOG_DEBUG("Dopamine RPE signal: " + std::to_string(error) + 
                  " (influence: " + std::to_string(predictionErrorInfluence) + ")");
}

} // namespace nlm
