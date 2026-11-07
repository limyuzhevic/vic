#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ========== ACETYLCHOLINE (ACh) ==========
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulator;
    float memoryConsolidationBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             attentionModulator(1.0f), memoryConsolidationBoost(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const { return "ACh"; }

float Acetylcholine::getLevel() const { return pImpl->level; }

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-relevant learning
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(bool attended) {
    // Bursty ACh signal when attention is captured
    if (attended) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.8f);
    }
}

void Acetylcholine::boostMemoryConsolidation() {
    // ACh enhances memory consolidation during rest
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f);
    pImpl->memoryConsolidationBoost = 1.5f;  // Boost consolidation efficiency
}

// ========== NORADRENALINE (NE) ==========
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    float focus;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousal(1.0f), vigilance(1.0f), focus(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const { return "NE"; }

float Norepinephrine::getLevel() const { return pImpl->level; }

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity during high arousal
    return 0.3f + 0.7f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE decays towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(bool highArousal) {
    // NE signals increased arousal
    if (highArousal) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.8f);
    }
}

void Norepinephrine::increaseVigilance() {
    // NE enhances vigilance and alertness
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.6f);
    pImpl->vigilance = 1.3f;  // Boost vigilance
}

const char* Serotonin::getName() const { return "5-HT"; }

float Serotonin::getLevel() const { return pImpl->level; }

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for mood-dependent learning
    return 0.7f - 0.4f * pImpl->level;  // Higher 5-HT = lower plasticity (stability)
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT decays towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalGoodMood() {
    // 5-HT increases with positive mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.7f);
    pImpl->mood = 1.4f;  // Boost mood
}

void Serotonin::signalSocialBonding() {
    // 5-HT signals social connection
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f);
    pImpl->socialBehavior = 1.2f;  // Enhance social behavior
}

} // namespace nlm