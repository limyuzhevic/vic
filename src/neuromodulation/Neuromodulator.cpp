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

// Acetylcholine: Attention and memory consolidation
// IMPLEMENTED - Phase 2
struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // ACh enhances plasticity for memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Burst of acetylcholine during attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate);
}

void Acetylcholine::signalNovelty(float noveltyLevel) {
    // ACh responds to novelty, enhancing encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyLevel * 0.5f * pImpl->releaseRate);
}

void Acetylcholine::enhanceMemoryConsolidation(float consolidationStrength) {
    // Enhance memory consolidation during rest
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * pImpl->releaseRate * 0.5f);
}

// Norepinephrine: Arousal and vigilance
// IMPLEMENTED - Phase 2
struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // NE enhances signal-to-noise ratio and plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    // Burst of norepinephrine during alertness/vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceLevel * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE responds to stress, enhancing arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate * 0.7f);
}

void Norepinephrine::enhanceSignalToNoise(float signalStrength) {
    // NE enhances signal-to-noise ratio for focused processing
    pImpl->level = std::min(pImpl->peak, pImpl->level + signalStrength * pImpl->releaseRate * 0.5f);
}

} // namespace nlm

// Acetylcholine: Attention and memory consolidation
// IMPLEMENTED - Phase 2
struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // ACh enhances plasticity for memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Burst of acetylcholine during attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate);
}

void Acetylcholine::signalNovelty(float noveltyLevel) {
    // ACh responds to novelty, enhancing encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyLevel * 0.5f * pImpl->releaseRate);
}

void Acetylcholine::enhanceMemoryConsolidation(float consolidationStrength) {
    // Enhance memory consolidation during rest
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * pImpl->releaseRate * 0.5f);
}

// Norepinephrine: Arousal and vigilance
// IMPLEMENTED - Phase 2
struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // NE enhances signal-to-noise ratio and plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    // Burst of norepinephrine during alertness/vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceLevel * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE responds to stress, enhancing arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate * 0.7f);
}

void Norepinephrine::enhanceSignalToNoise(float signalStrength) {
    // NE enhances signal-to-noise ratio for focused processing
    pImpl->level = std::min(pImpl->peak, pImpl->level + signalStrength * pImpl->releaseRate * 0.5f);
}

// Serotonin: Mood, impulsivity, and social behavior
// IMPLEMENTED - Phase 2
struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // 5-HT modulates plasticity for mood and decision making
    return 0.5f + 0.3f * pImpl->level - 0.2f * (pImpl->level * pImpl->level);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodLevel) {
    // Burst of serotonin during positive mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->releaseRate);
}

void Serotonin::signalImpulsivity(float impulsivityLevel) {
    // 5-HT inhibits impulsivity, lower levels mean higher impulsivity
    pImpl->level = std::max(0.0f, pImpl->level - impulsivityLevel * 0.3f * pImpl->releaseRate);
}

void Serotonin::signalRewardPrediction(float predictionLevel) {
    // 5-HT modulates reward prediction error
    pImpl->level = std::min(pImpl->peak, pImpl->level + predictionLevel * pImpl->releaseRate * 0.5f);
}

void Serotonin::enhanceSocialLearning(float socialStrength) {
    // 5-HT enhances social learning and behavior
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialStrength * pImpl->releaseRate * 0.7f);
}

} // namespace nlm
