#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float decisionBias;
    float impulsivityLevel;
    float rewardProcessing;
    float socialCohesion;
    float socialRecognition;
    float anxietyLevel;
    float sleepReadiness;
    float wakeDrive;
    float baselineMood;
    float decayRate;
    float releaseRate;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , decisionBias(0.5f)
        , impulsivityLevel(0.5f)
        , rewardProcessing(0.5f)
        , socialCohesion(0.5f)
        , socialRecognition(0.5f)
        , anxietyLevel(0.3f)
        , sleepReadiness(0.3f)
        , wakeDrive(0.7f)
        , baselineMood(0.3f)
        , decayRate(0.08f)
        , releaseRate(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity, with inverted U-curve relationship
    float basePlasticity = 0.5f;
    float modulation = pImpl->level;
    // Inverted U: moderate levels enhance, high levels reduce plasticity
    return basePlasticity + 0.8f * modulation * (1.0f - modulation * 0.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baselineMood,
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate related functions based on current level
    pImpl->decisionBias = 0.5f + pImpl->level * 0.3f;
    pImpl->impulsivityLevel = 1.0f - pImpl->level * 0.6f;  // Inverse relationship
    pImpl->rewardProcessing = 0.5f + pImpl->level * 0.5f;
    pImpl->socialCohesion = 0.5f + pImpl->level * 0.5f;
    pImpl->socialRecognition = 0.5f + pImpl->level * 0.5f;
    pImpl->anxietyLevel = 0.3f + pImpl->level * 0.4f;
    pImpl->sleepReadiness = 0.3f + pImpl->level * 0.6f;
    pImpl->wakeDrive = 0.7f - pImpl->level * 0.4f;  // Inverse relationship
}

void Serotonin::signalMood(float moodLevel) {
    // Mood enhancement signaling
    pImpl->level = std::min(1.0f, pImpl->level + moodLevel * pImpl->releaseRate * 0.6f);
}

void Serotonin::signalMotivation(float motivationLevel) {
    // Motivation enhancement signaling
    pImpl->level = std::min(1.0f, pImpl->level + motivationLevel * pImpl->releaseRate * 0.5f);
}

void Serotonin::signalSocialBehavior(float socialActivity) {
    // Social behavior enhancement
    pImpl->level = std::min(1.0f, pImpl->level + socialActivity * pImpl->releaseRate * 0.4f);
    pImpl->socialCohesion = std::min(1.0f, pImpl->socialCohesion + socialActivity * 0.1f);
    pImpl->socialRecognition = std::min(1.0f, pImpl->socialRecognition + socialActivity * 0.1f);
}

float Serotonin::getDecisionBias() const {
    return pImpl->decisionBias;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getRewardProcessing() const {
    return pImpl->rewardProcessing;
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

float Serotonin::getSocialRecognition() const {
    return pImpl->socialRecognition;
}

float Serotonin::getAnxietyLevel() const {
    return pImpl->anxietyLevel;
}

float Serotonin::getSleepReadiness() const {
    return pImpl->sleepReadiness;
}

float Serotonin::getWakeDrive() const {
    return pImpl->wakeDrive;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->decisionBias = 0.5f;
    pImpl->impulsivityLevel = 0.5f;
    pImpl->rewardProcessing = 0.5f;
    pImpl->socialCohesion = 0.5f;
    pImpl->socialRecognition = 0.5f;
    pImpl->anxietyLevel = 0.3f;
    pImpl->sleepReadiness = 0.3f;
    pImpl->wakeDrive = 0.7f;
}

} // namespace nlm