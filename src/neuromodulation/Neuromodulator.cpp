// Neuromodulator implementation
#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // ACh enhances plasticity and attention
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh promotes attention and memory consolidation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention() {
    // ACh boosts attentional focus and signal-to-noise ratio
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
}

void Acetylcholine::triggerMemoryConsolidation() {
    // ACh facilitates memory consolidation during sleep/rest
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // NE modulates arousal and plasticity
    return 1.0f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE regulates arousal and vigilance
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal() {
    // NE increases alertness and sensory processing
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f);
}

void Norepinephrine::enhanceVigilance() {
    // NE improves focus and sustained attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // 5-HT modulates mood and social behavior, can inhibit plasticity
    return 1.0f - 0.2f * pImpl->level;  // Higher 5-HT reduces plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT regulates mood and social behavior
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalPositiveArousal() {
    // 5-HT increases positive affect and social engagement
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
}

void Serotonin::signalNegativeArousal() {
    // 5-HT increases negative affect and withdrawal
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
}

} // namespace nlm