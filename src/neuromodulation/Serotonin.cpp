#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.12f)
        , releaseRate(1.2f)
        , mood(0.5f)
        , impulsivity(0.5f) {}
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
    // Serotonin modulates plasticity for mood and social behavior
    return 0.5f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::modulateMood(float moodLevel) {
    // Serotonin modulates mood state
    pImpl->mood = std::clamp(moodLevel, 0.0f, 1.0f);
    if (pImpl->mood > 0.7f) {
        // Positive mood increases serotonin level
        setLevel(std::min(pImpl->peak, getLevel() + 0.15f * pImpl->releaseRate));
    }
}

void Serotonin::modulateImpulsivity(float impulsivityLevel) {
    // Serotonin reduces impulsivity
    pImpl->impulsivity = std::clamp(impulsivityLevel, 0.0f, 1.0f);
    if (pImpl->impulsivity < 0.3f) {
        // Low impulsivity increases serotonin level
        setLevel(std::min(pImpl->peak, getLevel() + 0.1f * pImpl->releaseRate));
    }
}

void Serotonin::enhanceSocialBehavior(SocialLearning* socialLearning, float socialBoost) {
    if (!socialLearning) return;
    
    // Serotonin enhances social learning and cooperation
    float socialEffect = pImpl->level * socialBoost;
    socialLearning->setImitationStrength(std::min(1.0f, socialLearning->getImitationStrength() + socialEffect * 0.2f));
}

void Serotonin::integrateWithDopamine(float dopamineLevel) {
    // Serotonin and dopamine have complementary effects - serotonin for regulation, dopamine for reward
    if (dopamineLevel > 0.7f && pImpl->level > 0.5f) {
        // High dopamine + high serotonin = balanced reward processing
        setLevel(std::min(pImpl->peak, getLevel() + 0.1f));
    }
}

void Serotonin::integrateWithAcetylcholine(float achLevel) {
    // Serotonin and ACh have complementary effects - serotonin for mood regulation, ACh for attention
    if (achLevel > 0.7f && pImpl->level > 0.5f) {
        // High ACh + high serotonin = optimal cognitive state
        setLevel(1.0f);
    }
}

void Serotonin::integrateWithNorepinephrine(float neLevel) {
    // Serotonin and NE have complementary effects - serotonin for mood stability, NE for arousal
    if (neLevel > 0.7f && pImpl->level > 0.5f) {
        // High NE + high serotonin = balanced emotional-arousal state
        setLevel(std::min(pImpl->peak, getLevel() + 0.1f));
    }
}

} // namespace nlm
