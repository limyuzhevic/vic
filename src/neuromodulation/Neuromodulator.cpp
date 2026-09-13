#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention modulation
    float attentionStrength;
    float memoryEnhancement;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             attentionStrength(1.0f), memoryEnhancement(1.0f) {}
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
    // ACh enhances plasticity and memory consolidation
    return 0.5f + 0.5f * pImpl->level * (pImpl->attentionStrength + pImpl->memoryEnhancement) * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionStrength = std::max(0.1f, pImpl->attentionStrength - pImpl->decayRate * static_cast<float>(dt));
    pImpl->memoryEnhancement = std::max(0.1f, pImpl->memoryEnhancement - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::modulateAttention(float strength) {
    pImpl->attentionStrength = std::min(2.0f, pImpl->attentionStrength + strength);
}

void Acetylcholine::enhanceMemoryConsolidation(float strength) {
    pImpl->memoryEnhancement = std::min(2.0f, pImpl->memoryEnhancement + strength);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance
    float vigilance;
    float arousal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             vigilance(1.0f), arousal(1.0f) {}
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
    // NE modulates attention and arousal, which affects plasticity
    return 0.5f + 0.5f * pImpl->level * (pImpl->vigilance + pImpl->arousal) * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->vigilance = std::max(0.1f, pImpl->vigilance - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousal = std::max(0.1f, pImpl->arousal - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseVigilance(float strength) {
    pImpl->vigilance = std::min(2.0f, pImpl->vigilance + strength);
}

void Norepinephrine::modulateArousal(float strength) {
    pImpl->arousal = std::min(2.0f, pImpl->arousal + strength);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and behavioral modulation
    float mood;
    float impulsivity;
    float socialBehavior;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             mood(1.0f), impulsivity(1.0f), socialBehavior(1.0f) {}
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
    // Serotonin modulates mood and impulsivity, affecting learning rate
    return 0.5f + 0.5f * pImpl->level * (pImpl->mood + pImpl->impulsivity) * 0.5f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->mood = std::max(0.1f, pImpl->mood - pImpl->decayRate * static_cast<float>(dt));
    pImpl->impulsivity = std::max(0.1f, pImpl->impulsivity - pImpl->decayRate * static_cast<float>(dt));
    pImpl->socialBehavior = std::max(0.1f, pImpl->socialBehavior - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::modulateMood(float strength) {
    pImpl->mood = std::min(2.0f, pImpl->mood + strength);
}

void Serotonin::controlImpulsivity(float strength) {
    pImpl->impulsivity = std::min(2.0f, pImpl->impulsivity + strength);
}

void Serotonin::influenceSocialBehavior(float strength) {
    pImpl->socialBehavior = std::min(2.0f, pImpl->socialBehavior + strength);
}

struct Serotonin2A::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Neural gain and metaplasticity
    float neuralGain;
    float metaplasticity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             neuralGain(1.0f), metaplasticity(1.0f) {}
};

Serotonin2A::Serotonin2A() : pImpl(new Impl) {}

Serotonin2A::~Serotonin2A() = default;

const char* Serotonin2A::getName() const {
    return "5-HT2A";
}

float Serotonin2A::getLevel() const {
    return pImpl->level;
}

void Serotonin2A::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin2A::getPlasticityFactor() const {
    // 5-HT2A enhances neural gain and metaplasticity
    return 0.5f + 0.5f * pImpl->level * (pImpl->neuralGain + pImpl->metaplasticity) * 0.5f;
}

void Serotonin2A::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->neuralGain = std::max(0.1f, pImpl->neuralGain - pImpl->decayRate * static_cast<float>(dt));
    pImpl->metaplasticity = std::max(0.1f, pImpl->metaplasticity - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin2A::increaseNeuralGain(float strength) {
    pImpl->neuralGain = std::min(2.0f, pImpl->neuralGain + strength);
}

void Serotonin2A::modulateMetaplasticity(float strength) {
    pImpl->metaplasticity = std::min(2.0f, pImpl->metaplasticity + strength);
}

} // namespace nlm
