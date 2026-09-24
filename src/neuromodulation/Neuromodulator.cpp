#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float memoryConsolidationBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.5f), 
             attentionBoost(1.2f), memoryConsolidationBoost(1.3f) {}
};

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodMultiplier;
    float impulsivityReducer;
    float socialBehaviorModifier;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.12f), releaseRate(0.8f),
             moodMultiplier(0.9f), impulsivityReducer(0.7f), socialBehaviorModifier(1.1f) {}
};

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalMultiplier;
    float vigilanceMultiplier;
    float focusEnhancer;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(2.0f),
             arousalMultiplier(1.1f), vigilanceMultiplier(1.2f), focusEnhancer(1.3f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    NLM_LOG_INFO("Dopamine neuromodulator initialized");
}

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    NLM_LOG_INFO("Acetylcholine neuromodulator initialized");
}

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    NLM_LOG_INFO("Serotonin neuromodulator initialized");
}

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    NLM_LOG_INFO("Norepinephrine neuromodulator initialized");
}

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

const char* Serotonin::getName() const {
    return "5-HT";
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates attention-related plasticity
    // PLACEHOLDER: Moderate boost to plasticity
    return 1.0f + 0.3f * pImpl->level;
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT regulates learning stability
    // PLACEHOLDER: Higher serotonin reduces plasticity for stability
    return std::max(0.3f, 1.0f - 0.5f * pImpl->level);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal-dependent plasticity
    // PLACEHOLDER: Higher arousal increases plasticity
    return 0.8f + 0.4f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics for attention and memory consolidation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics for mood regulation and learning stability
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics for arousal and vigilance
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Reward signaling and prediction error for dopamine
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

// Acetylcholine signaling for attention and memory
void Acetylcholine::signalAttention(float attention) {
    // ACh increases with attention demands
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryConsolidation(float memorySignal) {
    // ACh boosts memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memorySignal * pImpl->releaseRate * 0.5f);
}

// Serotonin signaling for mood and social behavior
void Serotonin::signalMood(float mood) {
    // 5-HT responds to mood states
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * pImpl->releaseRate);
}

void Serotonin::signalSocialInteraction(float socialValue) {
    // 5-HT modulates social behavior based on interaction quality
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialValue * pImpl->releaseRate * 0.8f);
}

// Norepinephrine signaling for arousal and vigilance
void Norepinephrine::signalArousal(float arousal) {
    // NE increases with arousal demands
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate);
}

void Norepinephrine::signalVigilance(float vigilance) {
    // NE enhances vigilance and focus
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilance * pImpl->releaseRate * 1.2f);
}

void Norepinephrine::signalFocus(float focus) {
    // NE enhances cognitive focus and attentional selection
    pImpl->level = std::min(pImpl->peak, pImpl->level + focus * pImpl->releaseRate * 1.1f);
}

} // namespace nlm