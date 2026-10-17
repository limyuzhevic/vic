#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float memoryEnhancement;
    float attentionBoost;
    float decayRate;
    
    Impl() : level(0.0f), memoryEnhancement(1.5f), attentionBoost(2.0f), decayRate(0.1f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances memory-related plasticity
    return 1.0f + pImpl->memoryEnhancement * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards zero
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::strengthenMemory(float factor) {
    // Enhance memory consolidation
    pImpl->memoryEnhancement *= factor;
}

void Acetylcholine::enhanceAttention(float focus) {
    // Boost attention mechanisms
    pImpl->attentionBoost *= focus;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float vigilance;
    float sensoryGain;
    float arousal;
    float decayRate;
    
    Impl() : level(0.0f), vigilance(1.5f), sensoryGain(1.8f), arousal(1.2f), decayRate(0.1f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine increases vigilance and learning
    return 1.0f + pImpl->vigilance * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay with arousal modulation
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * (1.0f + pImpl->arousal * 0.2f));
}

void Norepinephrine::increaseVigilance(float alertness) {
    pImpl->vigilance *= alertness;
}

void Norepinephrine::enhanceSensoryProcessing(float gain) {
    pImpl->sensoryGain *= gain;
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float moodRegulation;
    float impulseControl;
    float socialBehavior;
    float decayRate;
    
    Impl() : level(0.0f), moodRegulation(1.2f), impulseControl(0.8f), socialBehavior(1.0f), decayRate(0.1f) {}
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
    // Serotonin regulates mood and impulse control
    return 1.0f + pImpl->moodRegulation * pImpl->level - pImpl->impulseControl * pImpl->level * 0.5f;
}

void Serotonin::update(TimestepDuration dt) {
    // Complex decay with social modulation
    float socialMod = 1.0f + pImpl->socialBehavior * pImpl->level * 0.3f;
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * socialMod);
}

void Serotonin::modulateMood(float mood) {
    pImpl->moodRegulation *= mood;
}

void Serotonin::regulateImpulsivity(float control) {
    pImpl->impulseControl *= control;
}

void Serotonin::enhanceSocialBehavior(float socialFactor) {
    pImpl->socialBehavior *= socialFactor;
}

// Histamine implementation
struct Histamine::Impl {
    float level;
    float wakefulness;
    float sleepRegulation;
    float cognitiveTone;
    float decayRate;
    
    Impl() : level(0.0f), wakefulness(1.8f), sleepRegulation(0.3f), cognitiveTone(1.3f), decayRate(0.1f) {}
};

Histamine::Histamine() : pImpl(new Impl) {}

Histamine::~Histamine() = default;

float Histamine::getLevel() const {
    return pImpl->level;
}

void Histamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Histamine::getPlasticityFactor() const {
    // Histamine balances wake-sleep states
    return 1.0f + pImpl->wakefulness * pImpl->level - pImpl->sleepRegulation * pImpl->level * 0.7f;
}

void Histamine::update(TimestepDuration dt) {
    // Wake-sleep cycle regulation
    float wakeSleepBalance = pImpl->wakefulness + pImpl->sleepRegulation;
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt) / wakeSleepBalance);
}

void Histamine::promoteWake(float arousal) {
    pImpl->wakefulness *= arousal;
}

void Histamine::induceSleep(float calmness) {
    pImpl->sleepRegulation *= calmness;
}

// Endorphin implementation
struct Endorphin::Impl {
    float level;
    float painRelief;
    float pleasureEnhancement;
    float stressReduction;
    float decayRate;
    
    Impl() : level(0.0f), painRelief(1.5f), pleasureEnhancement(1.3f), stressReduction(1.2f), decayRate(0.1f) {}
};

Endorphin::Endorphin() : pImpl(new Impl) {}

Endorphin::~Endorphin() = default;

float Endorphin::getLevel() const {
    return pImpl->level;
}

void Endorphin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Endorphin::getPlasticityFactor() const {
    // Endorphins balance pain and pleasure states
    return 1.0f + pImpl->painRelief * pImpl->level + pImpl->pleasureEnhancement * pImpl->level * 0.8f - pImpl->stressReduction * pImpl->level * 0.3f;
}

void Endorphin::update(TimestepDuration dt) {
    // Stress-responsive decay
    float stressResponse = 1.0f + pImpl->stressReduction * pImpl->level * 0.5f;
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * stressResponse);
}

void Endorphin::reducePain(float relief) {
    pImpl->painRelief *= relief;
}

void Endorphin::enhancePleasure(float reward) {
    pImpl->pleasureEnhancement *= reward;
}

} // namespace nlm
