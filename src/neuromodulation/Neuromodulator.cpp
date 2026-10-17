#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ========== ACh Implementation ========== //

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulator;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), attentionModulator(1.0f) {}
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
    // ACh modulates plasticity for memory consolidation
    // Higher ACh increases memory consolidation and working memory
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention modulation based on ACh level
    pImpl->attentionModulator = 0.5f + 0.5f * pImpl->level;
}

// ========== NE Implementation ========== //

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f),
             releaseRate(1.0f), arousal(1.0f), vigilance(1.0f) {}
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
    // NE modulates plasticity for arousal and learning
    // Higher NE increases attention and memory encoding
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal and vigilance based on NE level
    pImpl->arousal = 0.5f + 0.5f * pImpl->level;
    pImpl->vigilance = 0.5f + 0.5f * pImpl->level;
}

// ========== 5-HT Implementation ========== //

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f),
             releaseRate(1.0f), mood(0.5f), impulsivity(0.5f) {}
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
    // 5-HT modulates plasticity for mood and social behavior
    // Higher 5-HT reduces impulsivity and promotes stability
    return 0.5f - 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood and impulsivity based on 5-HT level
    pImpl->mood = 0.5f + 0.5f * pImpl->level;
    pImpl->impulsivity = 0.5f - 0.4f * pImpl->level;  // Inversely related
}

// ========== Neuromodulator Helper Functions ========== //

// Connect neuromodulators to brain systems
void setupNeuromodulationConnections(Brain* brain) {
    if (!brain) return;
    
    // Get neuromodulation systems
    auto* dopamine = brain->getDopamine();
    auto* acetylcholine = brain->getDopamine();  // TODO: Create ACh accessor
    auto* norepinephrine = brain->getDopamine(); // TODO: Create NE accessor
    auto* serotonin = brain->getDopamine();     // TODO: Create 5-HT accessor
    
    if (dopamine) {
        // Connect dopamine to working memory
        // Higher dopamine enhances working memory retention
        // This would be implemented in a real system
    }
    
    if (acetylcholine) {
        // Connect ACh to attention
        // ACh enhances focused attention and working memory
        // This would be implemented in a real system
    }
    
    if (norepinephrine) {
        // Connect NE to arousal
        // NE increases alertness and attention
        // This would be implemented in a real system
    }
    
    if (serotonin) {
        // Connect 5-HT to impulse control
        // 5-HT reduces impulsivity and promotes stable behavior
        // This would be implemented in a real system
    }
}

} // namespace nlm
