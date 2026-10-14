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

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionModulator;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), attentionModulator(2.0f) {}
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
    // ACh enhances plasticity for attention-related learning
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays toward baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::modulateAttention(float attentionLevel) {
    // ACh modulates neural excitability and signal-to-noise ratio
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionLevel * pImpl->attentionModulator);
}

void Acetylcholine::consolidateMemory(float consolidationStrength) {
    // ACh facilitates memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * 0.5f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalModulator;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), arousalModulator(3.0f) {}
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
    // NE enhances plasticity for arousal-related learning
    return 1.0f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE decays toward baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float arousalLevel) {
    // NE increases alertness and vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->arousalModulator);
}

void Norepinephrine::handleStressResponse(float stressLevel) {
    // NE responds to stress and increases vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * 2.0f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodModulator;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.03f), moodModulator(1.5f) {}
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
    // 5-HT generally reduces plasticity for stability, but enhances for mood-related learning
    return 1.0f - 0.4f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT decays slowly toward baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::modulateMood(float moodLevel) {
    // 5-HT modulates mood and emotional state
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->moodModulator);
}

void Serotonin::modulateImpulsivity(float impulsivityLevel) {
    // 5-HT reduces impulsivity and promotes patience
    pImpl->level = std::max(pImpl->baseline, pImpl->level - impulsivityLevel * 0.5f);
}

void Serotonin::modulateSocialBehavior(float socialCohesion) {
    // 5-HT enhances social bonding and cooperation
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialCohesion * 0.8f);
}

} // namespace nlm
