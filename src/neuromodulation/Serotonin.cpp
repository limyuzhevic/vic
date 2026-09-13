#pragma once

#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivity;
    float socialBehavior;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.04f), releaseRate(1.0f), moodLevel(0.0f),
             impulsivity(0.0f), socialBehavior(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized (Mood and Social Behavior)");
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
    // 5-HT modulates plasticity for mood and social behavior
    // Higher 5-HT stabilizes plasticity and reduces impulsivity
    float moodEffect = pImpl->moodLevel * 0.4f;
    float impulsivityEffect = pImpl->impulsivity * 0.3f;
    float socialEffect = pImpl->socialBehavior * 0.2f;
    return 0.5f - moodEffect - impulsivityEffect + socialEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Smooth mood and social behavior signals
    pImpl->moodLevel += (pImpl->level - pImpl->moodLevel) * 0.15f;
    pImpl->impulsivity += pImpl->moodLevel * 0.02f;
    pImpl->socialBehavior += pImpl->level * 0.03f;
}

// Biological mechanisms for 5-HT
void Serotonin::improveMood(float positiveInput) {
    // Improve mood in response to positive experiences
    float moodBoost = positiveInput * pImpl->releaseRate * 0.6f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodBoost);
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - positiveInput * 0.3f);
}

void Serotonin::reduceImpulsivity() {
    // Reduce impulsivity through serotonergic modulation
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - pImpl->level * 0.5f);
}

void Serotonin::modulateSocialBehavior(float socialContext) {
    // Modulate social behavior based on context
    // Positive social context increases 5-HT
    if (socialContext > 0.5f) {
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + socialContext * 0.4f);
    }
}

void Serotonin::regulateAggression(float threatLevel) {
    // Regulate aggression in response to threats
    float aggressionResponse = threatLevel * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + aggressionResponse);
    // 5-HT typically inhibits aggression
    pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + threatLevel * 0.3f);
}

} // namespace nlm
