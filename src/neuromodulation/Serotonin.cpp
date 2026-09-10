#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood-related parameters
    float moodState;
    float anxietyLevel;
    
    // Learning-related parameters
    float learningRateModulation;
    float predictionErrorSensitivity;
    
    // Sleep-related parameters
    bool isSleepPhase;
    float sleepHomeostasis;
    float wakeDrive;
    
    // Appetite parameters
    float appetiteLevel;
    
    Impl() : level(0.0f), baseline(0.03f), peak(1.0f), decayRate(0.03f), releaseRate(1.0f),
             moodState(0.5f), anxietyLevel(0.5f), learningRateModulation(1.0f),
             predictionErrorSensitivity(1.0f), isSleepPhase(false), sleepHomeostasis(1.0f),
             wakeDrive(1.0f), appetiteLevel(0.5f) {}
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
    // Update mood and anxiety based on level
    pImpl->moodState = 0.5f + pImpl->level * 1.5f;
    pImpl->anxietyLevel = std::max(0.0f, 1.0f - pImpl->level);
    pImpl->learningRateModulation = 0.8f + pImpl->level * 0.4f;
    pImpl->predictionErrorSensitivity = 1.0f + pImpl->level * 0.3f;
    pImpl->appetiteLevel = 0.5f + pImpl->level * 0.8f;
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity based on mood and anxiety
    float anxietyFactor = pImpl->anxietyLevel > 0.7f ? 0.8f : 1.0f;
    return 0.5f + 0.5f * pImpl->level * anxietyFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update sleep homeostasis
    if (!pImpl->isSleepPhase) {
        // Wake: increase sleep pressure
        pImpl->sleepHomeostasis = std::min(1.0f, pImpl->sleepHomeostasis + 0.001f * static_cast<float>(dt));
    } else {
        // Sleep: decrease sleep pressure
        pImpl->sleepHomeostasis = std::max(0.0f, pImpl->sleepHomeostasis - 0.002f * static_cast<float>(dt));
    }
    
    // Wake drive depends on sleep homeostasis
    pImpl->wakeDrive = 1.0f + pImpl->sleepHomeostasis * 0.5f;
}

void Serotonin::modulateMood(float amount) {
    // Modulate mood state
    pImpl->moodState = std::clamp(pImpl->moodState + amount * 0.1f, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate * 0.7f);
}

void Serotonin::modulateLearningRate(float amount) {
    // Modulate learning rate sensitivity
    pImpl->learningRateModulation = 1.0f + amount * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate * 0.5f);
}

void Serotonin::regulateSleep(bool isSleepPhase) {
    // Regulate sleep-wake cycle
    pImpl->isSleepPhase = isSleepPhase;
    if (isSleepPhase) {
        // Sleep phase: increase serotonin slightly
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
    } else {
        // Wake phase: serotonin may decrease
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f);
    }
}

} // namespace nlm