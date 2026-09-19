#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodEffect;  // Positive for positive mood, negative for negative mood
    float inhibitionStrength;  // Reduces impulsivity
    float flexibilityBoost;  // Enhances behavioral flexibility
    float socialEngagement;  // Affects social behavior
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(0.8f)
        , moodEffect(0.0f)
        , inhibitionStrength(0.0f)
        , flexibilityBoost(0.0f)
        , socialEngagement(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(class Brain* brain) {
    pImpl->brain = brain;
}

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
    // Serotonin modulates plasticity - low levels increase neural excitability and impulsivity
    // High levels stabilize networks and reduce impulsive behavior
    float stabilityFactor = 1.0f - pImpl->level * 0.5f;  // 1.0 at low 5-HT, 0.5 at high 5-HT
    
    // Also factor in mood effect
    float moodFactor = std::clamp(1.0f + pImpl->moodEffect * 0.3f, 0.5f, 2.0f);
    
    return stabilityFactor * moodFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood effect based on level
    pImpl->moodEffect = (pImpl->level - 0.5f) * 2.0f;  // -1 to +1 range
    
    // Update inhibition and flexibility based on level
    if (pImpl->level > 0.3f) {
        // Moderate to high levels reduce impulsivity
        pImpl->inhibitionStrength = (pImpl->level - 0.3f) * 1.5f;  // 0 to 1.05 range (capped at 1)
        pImpl->flexibilityBoost = (pImpl->level - 0.4f) * 1.25f;  // 0 to 0.75 range
        pImpl->socialEngagement = (pImpl->level - 0.5f) * 1.5f;  // -0.5 to 0.5 range
    } else {
        // Low levels increase impulsivity
        pImpl->inhibitionStrength = 0.0f;
        pImpl->flexibilityBoost = 0.0f;
        pImpl->socialEngagement = 0.0f;
    }
}

void Serotonin::reduceImpulsivity(float amount) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
}

void Serotonin::enhanceBehavioralFlexibility(float amount) {
    // Flexibility is tied to overall 5-HT level
    if (amount > 0.5f) {
        // Strong positive mood change
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
    } else if (amount < -0.5f) {
        // Strong negative mood change
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f);
    }
}

void Serotonin::respondToReward(float reward) {
    if (reward > 0.5f) {
        // Positive reward prediction error - increase serotonin
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
        // Enhance positive mood effect
        pImpl->moodEffect = std::max(pImpl->moodEffect, 0.3f);
    } else if (reward < -0.5f) {
        // Negative reward prediction error - decrease serotonin
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f);
        // Enhance negative mood effect
        pImpl->moodEffect = std::min(pImpl->moodEffect, -0.3f);
    }
}

void Serotonin::respondToPredictionError(float error) {
    // Prediction error also affects serotonin
    if (error > 0.5f) {
        // Unexpected positive outcome
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.05f);
        pImpl->flexibilityBoost = std::min(1.0f, pImpl->flexibilityBoost + 0.1f);
    } else if (error < -0.5f) {
        // Unexpected negative outcome
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f);
        pImpl->inhibitionStrength = std::max(0.0f, pImpl->inhibitionStrength - 0.1f);
    }
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodEffect = 0.0f;
    pImpl->inhibitionStrength = 0.0f;
    pImpl->flexibilityBoost = 0.0f;
    pImpl->socialEngagement = 0.0f;
}

} // namespace nlm