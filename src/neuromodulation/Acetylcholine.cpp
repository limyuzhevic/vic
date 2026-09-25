#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             attentionGain(1.0f), memoryBoost(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention and memory
    return 1.0f + pImpl->level * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float factor) {
    pImpl->attentionGain = factor;
    setLevel(std::min(1.0f, pImpl->level + 0.1f * factor));
}

void Acetylcholine::boostMemoryEncoding(float factor) {
    pImpl->memoryBoost = factor;
    setLevel(std::min(1.0f, pImpl->level + 0.05f * factor));
}

// ========== NORADRENALINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    float signalNoiseRatio;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             arousalLevel(1.0f), vigilanceLevel(1.0f), signalNoiseRatio(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances signal-to-noise ratio and plasticity
    return 1.0f + pImpl->level * 0.2f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline, with slower decay than ACh
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float level) {
    pImpl->arousalLevel = level;
    setLevel(std::min(1.0f, pImpl->level + 0.15f * level));
}

void Norepinephrine::enhanceVigilance(float focus) {
    pImpl->vigilanceLevel = focus;
    setLevel(std::min(1.0f, pImpl->level + 0.1f * focus));
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float impulseControl;
    float moodStability;
    float socialBehavior;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f),
             impulseControl(1.0f), moodStability(1.0f), socialBehavior(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() {
    delete pImpl;
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin stabilizes learning, reducing impulsivity
    return 1.0f / (1.0f + pImpl->level * 0.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline, moderate decay rate
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::reduceImpulsivity(float factor) {
    pImpl->impulseControl = factor;
    setLevel(std::min(1.0f, pImpl->level + 0.1f * factor));
}

void Serotonin::stabilizeMood(float stability) {
    pImpl->moodStability = stability;
    setLevel(std::min(1.0f, pImpl->level + 0.08f * stability));
}

} // namespace nlm
