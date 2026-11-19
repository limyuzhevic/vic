#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodGain;
    float impulsivityGain;
    float socialModulation;
    float rewardModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             moodGain(1.3f), impulsivityGain(0.8f), socialModulation(1.6f),
             rewardModulation(0.9f) {}
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
    // Serotonin modulates mood, impulsivity, and social behavior
    // Low serotonin increases impulsivity and negative mood effects
    float moodEffect = 1.0f - pImpl->level * 0.5f;  // 1.0 to 0.5
    float impulsivityEffect = pImpl->level * pImpl->impulsivityGain;  // 0.0 to ~1.0
    return moodEffect + impulsivityEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin decays naturally and responds to social feedback
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMoodChange(float moodChange) {
    // Positive mood increases serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodChange * pImpl->releaseRate);
}

void Serotonin::signalSocialFeedback(float socialFeedback) {
    // Social feedback (acceptance/rejection) modulates serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialFeedback * pImpl->socialModulation * pImpl->releaseRate);
}

void Serotonin::signalRewardEvaluation(float rewardEvaluation) {
    // Serotonin modulates reward evaluation and prediction
    pImpl->level = std::max(0.0f, pImpl->level + rewardEvaluation * pImpl->rewardModulation * pImpl->releaseRate);
}

} // namespace nlm
