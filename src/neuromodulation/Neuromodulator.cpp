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
    // Dopamine-modulated plasticity factor
    // Higher dopamine increases plasticity up to a maximum
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    // Cap at peak level and apply reward signal
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm

// Acetylcholine implementation

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionLevel(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    return 1.0f + 0.5f * pImpl->attentionLevel;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionLevel = std::max(0.0f, pImpl->attentionLevel - 0.01f * static_cast<float>(dt));
}

void Acetylcholine::enhanceMemory(float strength) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate);
}

void Acetylcholine::regulateAttention(float focus) {
    pImpl->attentionLevel = std::clamp(focus, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionLevel = 0.0f;
}

// Norepinephrine implementation

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float stressLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), arousalLevel(0.0f), stressLevel(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    float plasticity = 1.0f + 0.3f * pImpl->arousalLevel;
    if (pImpl->stressLevel > 0.5f) {
        plasticity *= (1.0f - (pImpl->stressLevel - 0.5f) * 0.5f);
    }
    return plasticity;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - 0.005f * static_cast<float>(dt));
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.003f * static_cast<float>(dt));
}

void Norepinephrine::increaseAlertness(float amount) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + amount);
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * 0.5f);
}

void Norepinephrine::decreaseStress(float amount) {
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - amount);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 0.0f;
    pImpl->stressLevel = 0.0f;
}

// Serotonin implementation

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), moodLevel(0.5f), impulsivity(0.5f) {}
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
    float plasticity = 1.0f + 0.3f * pImpl->moodLevel;
    plasticity *= (1.0f - 0.5f * pImpl->impulsivity);
    return std::max(0.0f, plasticity);
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->moodLevel = std::max(0.0f, pImpl->moodLevel - 0.01f * static_cast<float>(dt));
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - 0.005f * static_cast<float>(dt));
}

void Serotonin::improveMood(float amount) {
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + amount);
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * 0.5f);
}

void Serotonin::reduceImpulsivity(float amount) {
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - amount);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivity = 0.5f;
}

} // namespace nlm
