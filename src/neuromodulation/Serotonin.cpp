#include "Neuromodulator.hpp"
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
    
    // Serotonin parameters for mood, impulsivity, social behavior
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorLevel;
    float anxietyLevel;
    float satietyLevel;
    
    // 5-HT dynamics
    float serotoninSynthesized;
    float serotoninReleased;
    float synthesisRate;
    float clearanceRate;
    float reuptakeRate;
    
    // Behavioral modulation factors
    float rewardSensitivity;
    float punishmentSensitivity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , moodLevel(0.5f), impulsivityLevel(0.5f), socialBehaviorLevel(0.5f), anxietyLevel(0.0f), satietyLevel(0.0f)
        , serotoninSynthesized(0.0f), serotoninReleased(0.0f), synthesisRate(0.04f), clearanceRate(0.15f), reuptakeRate(0.1f)
        , rewardSensitivity(1.0f), punishmentSensitivity(1.0f) {}
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
    // Affects reward processing and behavioral flexibility
    return 0.8f - 0.3f * pImpl->level;  // Higher serotonin may reduce plasticity for stability
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline with reuptake
    float dt_float = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dt_float);
    
    // Reuptake rate depends on current level
    pImpl->reuptakeRate = 0.05f + 0.1f * pImpl->level;
    
    // Synthesize serotonin
    pImpl->serotoninSynthesized = pImpl->synthesisRate;
}

void Serotonin::signalMood(float mood) {
    // TODO PHASE 2: Implement mood signaling
    // PLACEHOLDER: Serotonin responds to mood signals
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + mood * 0.1f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * pImpl->releaseRate * 0.5f);
}

void Serotonin::signalReward(float reward) {
    // TODO PHASE 2: Implement reward modulation
    // PLACEHOLDER: Serotonin modulates reward sensitivity
    pImpl->rewardSensitivity = std::min(2.0f, pImpl->rewardSensitivity + reward * 0.1f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.3f);
}

void Serotonin::signalPunishment(float punishment) {
    // TODO PHASE 2: Implement punishment response
    // PLACEHOLDER: Punishment increases serotonin level (negative mood)
    pImpl->punishmentSensitivity = std::min(2.0f, pImpl->punishmentSensitivity + punishment * 0.1f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + punishment * pImpl->releaseRate * 0.8f);
}

void Serotonin::signalAnxiety(float anxiety) {
    // TODO PHASE 2: Implement anxiety response
    // PLACEHOLDER: Anxiety increases serotonin and affects social behavior
    pImpl->anxietyLevel = std::min(1.0f, pImpl->anxietyLevel + anxiety * 0.1f);
    pImpl->socialBehaviorLevel = std::max(0.0f, pImpl->socialBehaviorLevel - anxiety * 0.2f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + anxiety * pImpl->releaseRate * 0.7f);
}

void Serotonin::signalSatiety(float satiety) {
    // TODO PHASE 2: Implement satiety signaling
    // PLACEHOLDER: Satiety reduces serotonin response to food rewards
    pImpl->satietyLevel = std::min(1.0f, pImpl->satietyLevel + satiety * 0.1f);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - satiety * 0.05f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getSocialBehaviorLevel() const {
    return pImpl->socialBehaviorLevel;
}

float Serotonin::getAnxietyLevel() const {
    return pImpl->anxietyLevel;
}

float Serotonin::getSatietyLevel() const {
    return pImpl->satietyLevel;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivityLevel = 0.5f;
    pImpl->socialBehaviorLevel = 0.5f;
    pImpl->anxietyLevel = 0.0f;
    pImpl->satietyLevel = 0.0f;
    pImpl->serotoninSynthesized = 0.0f;
    pImpl->serotoninReleased = 0.0f;
    pImpl->rewardSensitivity = 1.0f;
    pImpl->punishmentSensitivity = 1.0f;
}

} // namespace nlm