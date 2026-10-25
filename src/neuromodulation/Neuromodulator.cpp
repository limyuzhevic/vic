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

} // namespace nlm

namespace nlm {

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionStrength;
    float memoryConsolidationStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             attentionStrength(1.0f), memoryConsolidationStrength(0.8f) {}
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
    return 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float strength) {
    // Boost attention modulation for salient stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * 0.5f);
}

void Acetylcholine::consolidateMemory(float relevance) {
    // Enhance memory consolidation for relevant experiences
    pImpl->level = std::min(pImpl->peak, pImpl->level + relevance * 0.3f);
}

void Acetylcholine::boostWorkingMemory(float capacityBoost) {
    // Increase working memory capacity
    pImpl->level = std::min(pImpl->peak, pImpl->level + capacityBoost * 0.2f);
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalBoost;
    float vigilanceStrength;
    float stressResponse;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f),
             arousalBoost(1.2f), vigilanceStrength(1.0f), stressResponse(0.5f) {}
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
    // NE increases plasticity for vigilance-related learning
    return 0.9f + 0.6f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline with stress adjustment
    float stressDecay = pImpl->stressResponse > 0.5f ? 1.2f : 1.0f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * stressDecay * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float arousal) {
    // Boost overall arousal and alertness
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->arousalBoost);
}

void Norepinephrine::enhanceVigilance(float strength) {
    // Enhance focus and vigilance for sustained attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->vigilanceStrength);
}

void Norepinephrine::respondToStress(float stressLevel) {
    // Stress response affects norepinephrine levels
    if (stressLevel > 0.5f) {
        pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + stressLevel * 0.3f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->stressResponse);
    } else {
        pImpl->stressResponse = std::max(0.0f, pImpl->stressResponse - 0.1f);
    }
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodRegulator;
    float impulseControl;
    float socialCohesion;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f),
             moodRegulator(1.0f), impulseControl(0.8f), socialCohesion(0.6f) {}
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
    // Serotonin modulates plasticity for mood-related learning
    return 0.7f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline with social modulation
    float socialBoost = pImpl->socialCohesion > 0.5f ? 0.9f : 1.0f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * socialBoost * static_cast<float>(dt));
}

void Serotonin::regulateMood(float moodShift) {
    // Boost or reduce mood-related serotonin levels
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodShift * pImpl->moodRegulator);
}

void Serotonin::enhanceImpulseControl(float strength) {
    // Enhance behavioral inhibition and impulse control
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->impulseControl);
}

void Serotonin::modulateSocialBehavior(float socialCohesion) {
    // Enhance or reduce social cohesion signals
    pImpl->socialCohesion = std::clamp(socialCohesion, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialCohesion * pImpl->socialCohesion);
}

} // namespace nlm
