#include "Serotonin.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // 5-HT effects on mood, impulsivity, and social behavior
    float moodLevel;
    float impulseControl;
    float socialMotivation;
    float anxietyLevel;
    
    // Plasticity effects
    float moodDependentPlasticity;
    float socialLearningStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f)
           , moodLevel(0.5f), impulseControl(0.5f), socialMotivation(0.5f), anxietyLevel(0.0f)
           , moodDependentPlasticity(0.3f), socialLearningStrength(0.4f) {}
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
    // 5-HT modulates mood, impulse control, and social behavior
    // Higher 5-HT improves mood stability and impulse control
    // Can have complex effects on plasticity (inverted U-shape relationship)
    return 1.0f + pImpl->level * pImpl->moodDependentPlasticity * (1.0f - pImpl->level * 0.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate impulse control based on 5-HT level
    // Higher 5-HT = better impulse control
    pImpl->impulseControl = 0.3f + pImpl->level * 0.7f;
    
    // Modulate social motivation
    // Higher 5-HT = higher social motivation (inverted U-shape)
    pImpl->socialMotivation = 0.2f + pImpl->level * 0.8f * (1.0f - pImpl->level * 0.3f);
    
    // Modulate anxiety level (inversely related to 5-HT)
    pImpl->anxietyLevel = 0.5f - pImpl->level * 0.5f;
}

void Serotonin::signalMoodBoost(float boost) {
    // Boost mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + boost * 0.5f);
}

void Serotonin::signalPositiveSocialInteraction(float strength) {
    // Enhance social motivation and learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate * 0.5f);
    pImpl->socialLearningStrength = std::min(1.0f, pImpl->socialLearningStrength + strength * 0.3f);
}

void Serotonin::signalNegativeSocialFeedback(float penalty) {
    // Negative social feedback reduces 5-HT level
    pImpl->level = std::max(0.0f, pImpl->level - penalty * pImpl->releaseRate * 2.0f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulseControl() const {
    return pImpl->impulseControl;
}

float Serotonin::getSocialMotivation() const {
    return pImpl->socialMotivation;
}

float Serotonin::getAnxietyLevel() const {
    return pImpl->anxietyLevel;
}

float Serotonin::getSocialLearningStrength() const {
    return pImpl->socialLearningStrength;
}

} // namespace nlm