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

// Acetylcholine implementation

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionStrength;
    float memoryConsolidationStrength;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), attentionStrength(1.0f), 
             memoryConsolidationStrength(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(std::make_unique<Impl>()) {}

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
    // Acetylcholine enhances plasticity for memory consolidation
    return 1.0f + 0.5f * pImpl->memoryConsolidationStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float strength) {
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength + strength);
}

void Acetylcholine::promoteMemoryConsolidation() {
    pImpl->memoryConsolidationStrength = std::min(1.0f, pImpl->memoryConsolidationStrength + 0.2f);
}

// Norepinephrine implementation

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalStrength;
    float vigilanceStrength;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), arousalStrength(1.0f), 
             vigilanceStrength(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()) {}

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
    // Norepinephrine modulates arousal and enhances attention
    return 1.0f + 0.3f * pImpl->arousalStrength + 0.2f * pImpl->vigilanceStrength;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float strength) {
    pImpl->arousalStrength = std::min(1.0f, pImpl->arousalStrength + strength);
}

void Norepinephrine::enhanceVigilance(float strength) {
    pImpl->vigilanceStrength = std::min(1.0f, pImpl->vigilanceStrength + strength);
}

// Serotonin implementation

struct Serotonin::Impl {
    float level;
    float baseline;
    float moodStrength;
    float socialBehaviorStrength;
    float impulseControlStrength;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), moodStrength(1.0f), 
             socialBehaviorStrength(1.0f), impulseControlStrength(1.0f), 
             decayRate(0.1f), releaseRate(1.0f) {}
};

Serotonin::Serotonin() : pImpl(std::make_unique<Impl>()) {}

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
    // Serotonin modulates mood and social behavior, affecting learning
    return 1.0f + 0.2f * pImpl->moodStrength + 0.3f * pImpl->socialBehaviorStrength;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::improveMood(float strength) {
    pImpl->moodStrength = std::min(1.0f, pImpl->moodStrength + strength);
}

void Serotonin::facilitateSocialBehavior(float strength) {
    pImpl->socialBehaviorStrength = std::min(1.0f, pImpl->socialBehaviorStrength + strength);
}

void Serotonin::enhanceImpulseControl(float strength) {
    pImpl->impulseControlStrength = std::min(1.0f, pImpl->impulseControlStrength + strength);
}

} // namespace nlm
