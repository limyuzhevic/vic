#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float impulseControlFactor;
    float socialRewardFactor;
    float behavioralInhibitionFactor;
    float satietyFactor;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), impulseControlFactor(1.0f), socialRewardFactor(1.2f),
             behavioralInhibitionFactor(0.8f), satietyFactor(0.7f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood, impulse control, and behavioral inhibition
    // Modulates plasticity based on behavioral outcomes and social context
    float basePlasticity = 0.5f;
    
    // Positive contribution from level
    basePlasticity += 0.5f * pImpl->level;
    
    // Inverse relationship with impulsivity
    basePlasticity *= (2.0f - pImpl->impulseControlFactor);
    
    // Enhance plasticity for socially rewarding behaviors
    if (pImpl->socialRewardFactor > 1.0f) {
        basePlasticity *= 1.1f;
    }
    
    return std::clamp(basePlasticity, 0.1f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline but modulate based on behavioral feedback
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Reward prediction - serotonin responds to reward prediction and satisfaction
void Serotonin::signalReward(float reward, float predictionError) {
    if (predictionError > 0.0f) {
        // Positive prediction error: unexpected reward increases serotonin
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * predictionError * pImpl->releaseRate);
    } else if (predictionError < -0.1f) {
        // Negative prediction error: reduces serotonin but with different dynamics
        pImpl->level = std::max(pImpl->baseline, pImpl->level - std::abs(predictionError) * 0.3f * pImpl->releaseRate);
    }
}

// Social behavior modulation - serotonin enhances prosocial behavior
void Serotonin::enhanceSocialBehavior(float socialCohesion, float fairness) {
    // Boost serotonin based on social context
    float boost = (socialCohesion * 0.6f + fairness * 0.4f) * pImpl->socialRewardFactor;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
}

// Behavioral inhibition - serotonin reduces impulsive actions
void Serotonin::inhibitImpulses(float inhibitionStrength) {
    // Increase level based on successful inhibition
    pImpl->level = std::min(pImpl->peak, pImpl->level + inhibitionStrength * pImpl->behavioralInhibitionFactor);
}

// Satiety signaling - serotonin indicates need states are satisfied
void Serotonin::signalSatiety(float satisfactionLevel) {
    // Moderate increase based on satiety
    pImpl->level = std::min(pImpl->peak, pImpl->level + satisfactionLevel * pImpl->satietyFactor);
}

} // namespace nlm