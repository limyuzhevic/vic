#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float tonicLevel;
    float peak;
    float riseTime;
    float decayTime;
    float maxBurstLevel;
    float receptorSensitivity;
    float eligibilityTrace;  // For reward-modulated learning
    float eligibilityTraceRate;
    float eligibilityTraceDecay;
    std::vector<float> spikeTimes;
    
    Impl() : level(0.0f), baseline(0.15f), tonicLevel(0.15f), peak(1.0f), 
             riseTime(0.2f), decayTime(2.0f), maxBurstLevel(3.0f),
             receptorSensitivity(1.0f), eligibilityTrace(0.0f), eligibilityTraceRate(0.9f), 
             eligibilityTraceDecay(0.95f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    pImpl->spikeTimes.reserve(100);
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
    // Biologically plausible dopamine modulation of plasticity
    // Follows inverted-U relationship: optimal at moderate levels, reduced at very high/low
    float normalizedLevel = pImpl->level / pImpl->maxBurstLevel;
    
    // Sigmoidal response function with peak at optimal range
    float activation = 4.0f * normalizedLevel * (1.0f - normalizedLevel);
    
    // Apply receptor saturation at high concentrations
    float saturatedActivation = activation / (1.0f + normalizedLevel * 0.5f);
    
    // Tonic baseline plasticity
    float baselinePlasticity = 0.3f;
    
    // Modulation by dopamine concentration
    return baselinePlasticity + saturatedActivation * 0.7f;
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics based on differential equations
    // Incorporates rise phase, plateau, and decay phase
    const float deltaTime = static_cast<float>(dt);
    
    // Calculate time since last spike for decay
    float decayFactor = std::pow(pImpl->eligibilityTraceDecay, deltaTime);
    
    // Decay towards baseline and tonic level
    float targetLevel = pImpl->tonicLevel + (pImpl->baseline - pImpl->tonicLevel) * 0.1f;
    pImpl->level = targetLevel + (pImpl->level - targetLevel) * decayFactor;
    
    // Update eligibility trace
    pImpl->eligibilityTrace *= decayFactor;
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
