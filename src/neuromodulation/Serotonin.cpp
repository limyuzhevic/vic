// Serotonin.cpp - Mood and behavioral regulation implementation

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float impulsivityModulator;
    float socialBehaviorModulator;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , impulsivityModulator(0.5f)
        , socialBehaviorModulator(0.5f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f) {}
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
    // Serotonin modulates plasticity and learning
    return 0.3f + pImpl->level * 0.4f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Reduce impulsivity (high 5-HT reduces impulsive actions)
void Serotonin::reduceImpulsivity(Brain* brain, float currentImpulsivity) {
    if (!brain || pImpl->level <= 0.0f) return;
    
    // Could modulate action selection based on impulsivity
    // Higher 5-HT levels reduce tendency toward impulsive actions
    float effectiveImpulsivity = currentImpulsivity * (1.0f - pImpl->impulsivityModulator * pImpl->level);
    
    NLM_LOG_DEBUG("Serotonin::reduceImpulsivity: Reduced impulsivity from " << currentImpulsivity 
                << " to " << effectiveImpulsivity << " at level " << pImpl->level);
}

// Modulate social behavior
void Serotonin::modulateSocialBehavior(Brain* brain, bool isSocialContext) {
    if (!brain) return;
    
    // In social contexts, higher serotonin promotes social approach behaviors
    if (isSocialContext && pImpl->level > 0.5f) {
        // Could enhance social learning and memory
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            // Social experiences are more likely to be consolidated
            episodicMem->consolidate(0.4f * pImpl->socialBehaviorModulator * pImpl->level);
        }
    }
}

// Signal mood change (e.g., reward, stress, social status)
void Serotonin::signalMoodChange(float moodDelta) {
    // Mood changes affect serotonin levels
    pImpl->level = std::clamp(pImpl->level + moodDelta * 0.3f, 0.0f, pImpl->peak);
}

} // namespace nlm
