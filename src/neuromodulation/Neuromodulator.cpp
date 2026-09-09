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

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2

struct Acetylcholine::Impl {
    float level;
    float attentionGain;
    float memoryConsolidation;
    float decayRate;
    float cholineAfferent;
    
    Impl() : level(0.0f), attentionGain(1.0f), memoryConsolidation(1.0f), 
             decayRate(0.05f), cholineAfferent(0.1f) {}
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
    // Acetylcholine modulates attention and working memory
    // Higher ACh increases signal-to-noise ratio
    return 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay and cholinergic input
    float cholinergicInput = pImpl->cholineAfferent * (1.0f - pImpl->level);
    pImpl->level += (cholinergicInput - pImpl->level * pImpl->decayRate) * static_cast<float>(dt);
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2

struct Norepinephrine::Impl {
    float level;
    float arousal;
    float locusCoeruleusInput;
    float decayRate;
    float burstFrequency;
    
    Impl() : level(0.0f), arousal(0.5f), locusCoeruleusInput(0.2f), 
             decayRate(0.03f), burstFrequency(0.1f) {}
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
    // Norepinephrine modulates arousal and stress response
    // Moderate NE is optimal for learning, high NE impairs consolidation
    return 0.6f + 0.3f * pImpl->level * (1.0f - pImpl->level);
}

void Norepinephrine::update(TimestepDuration dt) {
    // LC firing modulated by predictability and novelty
    float noveltyModulation = 0.5f * (1.0f - pImpl->level);  // More novel = more firing
    pImpl->level += (noveltyModulation - pImpl->level * pImpl->decayRate) * static_cast<float>(dt);
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2

struct Serotonin::Impl {
    float level;
    float mood;
    float impulsivity;
    float decayRate;
    float rapheInput;
    float socialReward;
    
    Impl() : level(0.5f), mood(0.5f), impulsivity(0.5f), 
             decayRate(0.04f), rapheInput(0.15f), socialReward(0.3f) {}
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
    // Serotonin modulates mood and social behavior
    // Balanced serotonin supports stable mood and learning
    return 0.7f + 0.2f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Activity-dependent serotonergic modulation
    float socialInput = pImpl->socialReward * (1.0f - pImpl->level);
    float moodFeedback = 0.1f * (pImpl->mood - 0.5f);  // Homeostasis
    pImpl->level += (socialInput + moodFeedback - pImpl->level * pImpl->decayRate) * static_cast<float>(dt);
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

} // namespace nlm
