#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float punishmentSensitivity;
    float rewardSensitivity;
    float socialFeedback;
    float baseline;
    float decayRate;
    float releaseRate;
    float learningRateModulator;
    
    Impl() : level(0.0f), punishmentSensitivity(0.0f), rewardSensitivity(0.0f), socialFeedback(0.0f), baseline(0.03f), decayRate(0.6f), releaseRate(2.5f), learningRateModulator(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates learning rate and punishment sensitivity (Sharp et al. 2015)
    // High serotonin decreases learning rate but increases stability
    // This implements a Bayesian integration of prediction errors
    
    float punishmentEffect = pImpl->punishmentSensitivity * 0.9f;  // Strong punishment effect
    float socialEffect = pImpl->socialFeedback * 0.6f;
    
    // Serotonin reduces plasticity for better stability
    float basePlasticity = 0.8f;  // Higher baseline (serotonin normally reduces plasticity)
    float reduction = punishmentEffect * 0.5f + socialEffect * 0.3f;
    
    float plasticityFactor = basePlasticity * (1.0f - reduction);
    
    return std::clamp(plasticityFactor, 0.2f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics based on Sharp et al. (2015) model
    float deltaT = static_cast<float>(dt) * 0.001f;
    
    // Decay towards baseline
    float decayFactor = std::exp(-pImpl->decayRate * deltaT);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Modulate learning rate based on current level
    pImpl->learningRateModulator = 1.0f / (1.0f + pImpl->level * 3.0f);  // High serotonin reduces learning rate
    
    // Ensure physiological bounds
    pImpl->level = std::max(0.0f, std::min(pImpl->level, 1.0f));
    pImpl->learningRateModulator = std::max(0.2f, std::min(pImpl->learningRateModulator, 1.0f));
}

void Serotonin::signalPunishment(float punishment) {
    // Serotonin responds to aversive stimuli and promotes behavioral inhibition
    float clampedPunishment = std::clamp(punishment, 0.0f, 1.0f);
    pImpl->punishmentSensitivity = clampedPunishment;
    
    // Increase serotonin level for punishment processing
    pImpl->level = std::min(1.0f, pImpl->level + clampedPunishment * pImpl->releaseRate * 4.0f);
    
    // Strong punishment triggers a sustained response
    if (clampedPunishment > 0.6f) {
        pImpl->level = std::min(1.0f, pImpl->level + 0.3f);
    }
}

void Serotonin::signalSocialFeedback(float feedback) {
    // Serotonin encodes social exclusion, acceptance, and relatedness
    float clampedFeedback = std::clamp(feedback, 0.0f, 1.0f);
    pImpl->socialFeedback = clampedFeedback;
    
    // Positive social feedback reduces serotonin (Cody et al. 2017)
    // Negative social feedback increases serotonin
    if (clampedFeedback < 0.3f) {
        pImpl->level = std::min(1.0f, pImpl->level + (0.3f - clampedFeedback) * pImpl->releaseRate * 3.0f);
    }
}

} // namespace nlm