#include "Serotonin.hpp"
#include <algorithm>
#include <random>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;                    // Current 5-HT level (0.0-2.0)
    float baseline;                  // Baseline tone
    float mood;                      // Current mood (-1.0 to 1.0)
    float motivation;                // Motivation level (0.0-1.0)
    float socialBehavior;            // Social behavior propensity (0.0-1.0)
    float rewardSensitivity;         // Reward processing sensitivity
    float impulsivity;               // Impulsivity level (0.0-1.0)
    float peakLevel;                 // Peak release level
    float decayRate;                 // Recovery to baseline
    float releaseRate;               // Release rate
    float reuptakeRate;              // Reuptake rate (negative)
    bool positiveContext;            // Current environmental context
    
    Impl() : level(0.0f), baseline(0.15f), mood(0.0f), motivation(0.5f),
             socialBehavior(0.5f), rewardSensitivity(1.0f), impulsivity(0.5f),
             peakLevel(2.5f), decayRate(0.03f), releaseRate(1.2f),
             reuptakeRate(-0.05f), positiveContext(true) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for emotional learning
    float moodInfluence = std::abs(pImpl->mood);
    return 1.0f - 0.3f * moodInfluence;
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay and reuptake
    pImpl->level += pImpl->reuptakeRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level);
    
    // Update mood based on context and level
    pImpl->mood = (pImpl->level - 1.0f) / 2.0f;  // Maps [0.0, 2.0] to [-0.5, 0.5]
    
    // Update motivation based on mood
    pImpl->motivation = 0.5f + 0.5f * std::max(0.0f, pImpl->mood);
}

float Serotonin::getMoodLevel() const {
    return pImpl->mood;
}

void Serotonin::signalMoodChange(float moodDelta, bool positiveEvent) {
    // Phasic response to mood-altering events
    pImpl->level = std::min(pImpl->peakLevel, pImpl->level + moodDelta * pImpl->releaseRate);
    pImpl->positiveContext = positiveEvent;
}

float Serotonin::getMotivationModulator() const {
    return pImpl->motivation;
}

void Serotonin::modulateMotivation(bool isRewardingContext) {
    pImpl->positiveContext = isRewardingContext;
    if (isRewardingContext) {
        pImpl->motivation = std::min(1.0f, pImpl->motivation + 0.2f);
    }
}

float Serotonin::getSocialBehaviorModulator() const {
    return pImpl->socialBehavior;
}

void Serotonin::influenceSocialBehavior(float socialContext) {
    // socialContext: -1.0 (negative) to 1.0 (positive)
    pImpl->socialBehavior = 0.5f + 0.5f * socialContext;
    if (pImpl->positiveContext && socialContext > 0.0f) {
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.2f);
    }
}

void Serotonin::improveMood() const {
    // Enhance mood-related processing and reduce negative affect
    // Implementation would modulate limbic system activity
}

void Serotonin::modulateRewardProcessing() const {
    // Adjust reward prediction and valuation
    // Implementation would interact with dopamine systems
}

void Serotonin::regulateImpulsivity() const {
    // Reduce impulsive behavior, enhance patience
    // Implementation would modulate prefrontal control
}

void Serotonin::enhanceSocialLearning() const {
    // Improve learning from social information
    // Implementation would enhance mirror neuron and social cognition systems
}

} // namespace nlm