#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float reuptakeRate;
    float receptorDensity;
    float receptorSaturation;
    float vesicleReleaseProbability;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(2.0f)
        , reuptakeRate(0.15f)
        , receptorDensity(1.0f)
        , receptorSaturation(0.7f)
        , vesicleReleaseProbability(0.8f) {}
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
    // Dopamine modulates plasticity through receptor activation
    float effectiveLevel = std::min(pImpl->level / pImpl->receptorSaturation, 1.0f);
    
    // D1 receptor promotes LTP, D2 receptor promotes LTD
    // Balance depends on concentration
    float d1Effect = effectiveLevel * 1.5f;  // Stronger LTP effect
    float d2Effect = effectiveLevel * 0.8f;  // Moderate LTD effect
    
    // Net plasticity factor: positive for learning enhancement
    float plasticityFactor = 1.0f + 0.5f * (d1Effect - d2Effect);
    
    return std::max(0.1f, std::min(plasticityFactor, 2.0f));
}

void Dopamine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Multiple decay processes
    float decay = pImpl->decayRate * dtFloat;
    
    // Natural enzymatic degradation
    float degradation = decay * 0.7f;
    
    // Reuptake by dopamine transporters (DAT)
    float reuptake = pImpl->reuptakeRate * pImpl->level * dtFloat;
    
    // Vesicular release clearance
    float vesicularRelease = (1.0f - pImpl->vesicleReleaseProbability) * decay * dtFloat;
    
    // Total clearance
    float totalClearance = degradation + reuptake + vesicularRelease;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - totalClearance);
}

void Dopamine::signalReward(float reward) {
    // Realistic dopamine response to rewards
    // Uses a burst-release mechanism
    
    // Probability of burst release based on reward prediction
    float burstProbability = std::min(1.0f, reward * pImpl->vesicleReleaseProbability);
    
    // Actual burst size (scaled by reward magnitude)
    float burstSize = reward * pImpl->releaseRate;
    
    // Apply burst with some noise (biological variability)
    float noise = (rand() % 100) / 100.0f * 0.1f - 0.05f;
    float actualBurst = burstSize * (1.0f + noise);
    
    // Apply to current level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + actualBurst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (Rescorla-Wagner style)
    // Phasic dopamine bursts for positive prediction errors (better-than-expected)
    // Dip for negative prediction errors (worse-than-expected)
    
    if (error > 0.0f) {
        // Positive prediction error: burst firing
        float burstSize = error * pImpl->releaseRate * 1.2f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize);
    } else {
        // Negative prediction error: dip in activity
        float dipSize = std::abs(error) * pImpl->reuptakeRate * 0.8f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dipSize);
    }
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm