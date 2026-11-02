#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f)
        , attentionBoost(1.5f), memoryBoost(2.0f) {}
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
    // ACh modulates both attention and memory consolidation
    // Higher ACh improves attention filtering and memory encoding
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: decay towards baseline, but can be boosted by attention demands
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionLevel) {
    // ACh release in response to attentional demands
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionLevel * pImpl->releaseRate * pImpl->attentionBoost);
}

void Acetylcholine::signalMemoryConsolidation(float memoryStrength) {
    // ACh supports memory consolidation during learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * pImpl->releaseRate * pImpl->memoryBoost);
}

// Norepinephrine: Arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalBoost;
    float vigilanceBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f)
        , arousalBoost(1.3f), vigilanceBoost(1.4f) {}
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
    // NE modulates arousal and vigilance, affecting overall learning readiness
    return 0.3f + 0.7f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: decay, but can be boosted by arousal and vigilance demands
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // NE release in response to salient stimuli and arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate * pImpl->arousalBoost);
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    // NE supports vigilance and focused attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceLevel * pImpl->releaseRate * pImpl->vigilanceBoost);
}

// Serotonin: Mood, impulsivity, and social behavior
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodBoost;
    float socialBoost;
    float impulsivityMod;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.03f), releaseRate(1.0f)
        , moodBoost(1.2f), socialBoost(1.4f), impulsivityMod(0.8f) {}
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
    // Serotonin modulates mood and social behavior, affecting learning stability
    return 0.4f + 0.6f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: slower decay, longer persistence
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodLevel) {
    // Serotonin release in response to positive experiences
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->releaseRate * pImpl->moodBoost);
}

void Serotonin::signalSocialExperience(float socialLevel) {
    // Serotonin supports social learning and bonding
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialLevel * pImpl->releaseRate * pImpl->socialBoost);
}

void Serotonin::modulateImpulsivity(float currentLevel, float& newLevel) {
    // Serotonin reduces impulsivity and promotes patience
    newLevel = currentLevel * pImpl->impulsivityMod;
}

} // namespace nlm
