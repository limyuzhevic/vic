#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Implement the remaining neuromodulators

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;
    float impulsivityModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.2f), 
             attentionModulation(1.0f), impulsivityModulation(1.0f) {}
};

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalLevel(1.0f), vigilanceLevel(1.0f) {}
};

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryConsolidation;
    float attentionFocus;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(0.9f),
             memoryConsolidation(1.0f), attentionFocus(1.0f) {}
};

// Serotonin Implementations
Serotonin::Serotonin() {
    pImpl = new Impl();
}

Serotonin::~Serotonin() {
    delete pImpl;
}

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
    // Serotonin modulates attention and reduces impulsivity
    // Higher serotonin reduces noise, stabilizes attention
    float attentionEffect = 1.0f + (pImpl->level * 0.5f);
    float impulsivityEffect = 1.0f / (1.0f + pImpl->level * 0.5f);
    
    return attentionEffect * impulsivityEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin affects attention and memory consolidation
    // Gradual decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate attention strength
    pImpl->attentionModulation = 1.0f / (1.0f + pImpl->level * 0.5f);
    
    // Reduce impulsivity with higher serotonin
    pImpl->impulsivityModulation = 1.0f - pImpl->level * 0.3f;
}

// Norepinephrine Implementations
Norepinephrine::Norepinephrine() {
    pImpl = new Impl();
}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

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
    // Norepinephrine enhances arousal and vigilance
    // Strong norepinephrine increases plasticity and learning rate
    return 0.5f + pImpl->level * 1.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Norepinephrine affects arousal and vigilance
    // Rapid decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Enhance arousal and vigilance
    pImpl->arousalLevel = 1.0f + pImpl->level * 0.8f;
    pImpl->vigilanceLevel = 1.0f + pImpl->level * 0.6f;
}

// Acetylcholine Implementations
Acetylcholine::Acetylcholine() {
    pImpl = new Impl();
}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

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
    // Acetylcholine primarily affects memory consolidation and attention
    // Enhances encoding and consolidation of new memories
    return 1.0f + pImpl->level * 0.8f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Acetylcholine affects memory consolidation and attentional focus
    // Gradual decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Enhance memory consolidation
    pImpl->memoryConsolidation = 1.0f + pImpl->level * 0.5f;
    
    // Focus attention
    pImpl->attentionFocus = 1.0f + pImpl->level * 0.4f;
}

} // namespace nlm