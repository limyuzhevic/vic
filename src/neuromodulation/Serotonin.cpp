#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float socialCohesionLevel;
    float impulseControl;
    float anhedoniaLevel;
    float rewardSensitivity;
    float punishmentSensitivity;
    
    Impl() 
        : level(0.0f), baseline(0.1f), peak(1.0f), 
          decayRate(0.06f), moodLevel(0.5f),
          socialCohesionLevel(0.5f), impulseControl(0.8f),
          anhedoniaLevel(0.0f), rewardSensitivity(1.0f),
          punishmentSensitivity(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin system initialized");
}

Serotonin::~Serotonin() {
    delete pImpl;
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
    // Serotonin affects mood and social behavior, indirectly affecting learning
    // Higher serotonin generally promotes stability and reduces rash decisions
    return 1.0f - 0.3f * pImpl->anhedoniaLevel + 0.2f * pImpl->moodLevel;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: slower, maintains mood baseline
    // Time constant ~350ms (decayRate = 0.06 per ms)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->moodLevel = pImpl->level;
}

void Serotonin::signalEvent(float magnitude) {
    // Events affect serotonin and mood
    if (magnitude > 0.0f) {
        // Positive events increase serotonin and mood
        pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * 0.8f);
        pImpl->rewardSensitivity = std::min(2.0f, pImpl->rewardSensitivity + magnitude * 0.2f);
        pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + magnitude * 0.3f);
    } else if (magnitude < 0.0f) {
        // Negative events decrease serotonin and increase anhedonia
        pImpl->level = std::max(pImpl->baseline, pImpl->level + magnitude * 0.5f);
        pImpl->anhedoniaLevel = std::min(1.0f, pImpl->anhedoniaLevel - magnitude * 0.3f);
        pImpl->punishmentSensitivity = std::min(2.0f, pImpl->punishmentSensitivity - magnitude * 0.3f);
    }
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->anhedoniaLevel = 0.0f;
    pImpl->socialCohesionLevel = 0.5f;
    pImpl->rewardSensitivity = 1.0f;
    pImpl->punishmentSensitivity = 0.5f;
}

float Serotonin::getBaselineLevel() const {
    return pImpl->baseline;
}

float Serotonin::getPeakLevel() const {
    return pImpl->peak;
}

void Serotonin::signalSocialCohesion(float cohesionLevel) {
    // Social cohesion affects serotonin levels
    pImpl->socialCohesionLevel = cohesionLevel;
    // High social cohesion increases serotonin
    if (cohesionLevel > 0.7f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + cohesionLevel * 0.3f);
        pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + cohesionLevel * 0.4f);
    }
}

void Serotonin::signalRewardPunishment(float reward, float punishment) {
    // Combined reward/punishment affects serotonin dynamics
    if (reward > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * 0.5f);
        pImpl->rewardSensitivity = std::min(2.0f, pImpl->rewardSensitivity + reward * 0.2f);
    }
    if (punishment > 0.0f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - punishment * 0.3f);
        pImpl->punishmentSensitivity = std::min(2.0f, pImpl->punishmentSensitivity + punishment * 0.3f);
        pImpl->anhedoniaLevel = std::min(1.0f, pImpl->anhedoniaLevel + punishment * 0.2f);
    }
}

void Serotonin::modulateImpulsivity(float controlLevel) {
    // Direct modulation of impulse control
    pImpl->impulseControl = std::clamp(controlLevel, 0.0f, 1.0f);
    // Lower impulse control (higher impulsivity) correlates with lower serotonin
    pImpl->level = std::max(pImpl->baseline, pImpl->level - (1.0f - controlLevel) * 0.2f);
}

void Serotonin::modulateAnhedonia(float anhedoniaLevel) {
    // Direct modulation of anhedonia
    pImpl->anhedoniaLevel = std::clamp(anhedoniaLevel, 0.0f, 1.0f);
    // Higher anhedonia correlates with lower serotonin
    pImpl->level = std::max(pImpl->baseline, pImpl->level - anhedoniaLevel * 0.3f);
    pImpl->moodLevel = std::max(0.0f, pImpl->moodLevel - anhedoniaLevel * 0.4f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getSocialCohesionLevel() const {
    return pImpl->socialCohesionLevel;
}

float Serotonin::getImpulseControlLevel() const {
    return pImpl->impulseControl;
}

} // namespace nlm
