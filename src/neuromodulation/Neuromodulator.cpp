#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionFocus;
    float memoryBoost;
    float arousalMod;
    
    Impl() : level(0.0f), baseline(0.1f), attentionFocus(0.5f), memoryBoost(0.3f), arousalMod(0.8f) {}
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
    // ACh enhances synaptic plasticity, especially for attention-focused connections
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: attention focus and memory consolidation
    pImpl->attentionFocus = pImpl->level * 0.8f;
    pImpl->memoryBoost = pImpl->level * 0.6f;
    pImpl->arousalMod = pImpl->level * 0.7f;
}

void Acetylcholine::signalAttention(float strength) {
    // Signal increased attention to specific neural populations
    setLevel(std::min(1.0f, pImpl->level + strength * 0.3f));
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Boost memory consolidation during attention focus
    setLevel(std::min(1.0f, pImpl->level + 0.2f));
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float vigilance;
    float stressResponse;
    float arousal;
    
    Impl() : level(0.0f), baseline(0.05f), vigilance(0.6f), stressResponse(0.4f), arousal(0.7f) {}
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
    // NE enhances alertness and modulates plasticity based on arousal
    return 1.0f + pImpl->level * 0.3f + (pImpl->level > 0.5f ? 0.2f : 0.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: vigilance and stress response
    pImpl->vigilance = pImpl->level * 0.7f;
    pImpl->stressResponse = pImpl->level * 0.5f;
    pImpl->arousal = pImpl->level * 0.8f;
}

void Norepinephrine::signalArousal(float intensity) {
    // Signal increased arousal and alertness
    setLevel(std::min(1.0f, pImpl->level + intensity * 0.4f));
}

void Norepinephrine::stressResponse(float stressor) {
    // Stress response increases NE levels
    setLevel(std::min(1.0f, pImpl->level + stressor * 0.5f));
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float mood;
    float impulsivity;
    float socialBehavior;
    
    Impl() : level(0.0f), baseline(0.1f), mood(0.5f), impulsivity(0.5f), socialBehavior(0.6f) {}
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
    // Serotonin modulates plasticity and emotional state
    float moodEffect = (pImpl->level - 0.5f) * 0.2f;  // Positive mood enhances plasticity
    float impulsivityEffect = (1.0f - pImpl->level) * 0.1f;  // High impulsivity reduces plasticity
    return 1.0f + moodEffect - impulsivityEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: mood and social behavior regulation
    pImpl->mood = pImpl->level * 0.6f;
    pImpl->impulsivity = (1.0f - pImpl->level) * 0.5f;
    pImpl->socialBehavior = pImpl->level * 0.7f;
}

void Serotonin::signalReward(float reward) {
    // Reward increases serotonin levels (positive affect)
    setLevel(std::min(1.0f, pImpl->level + reward * 0.3f));
}

void Serotonin::socialBonding(float bonding) {
    // Social bonding increases serotonin
    setLevel(std::min(1.0f, pImpl->level + bonding * 0.4f));
}
