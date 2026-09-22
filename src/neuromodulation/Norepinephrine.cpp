#pragma once

#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;                    // Current arousal level
    float locusCoeruleusGain;       // LC firing rate modulation
    float stressLevel;              // Stress response level
    float arousal;                 // Arousal level (0-1)
    float vigilance;                // Vigilance/alertness level
    float baseline;                 // Baseline level
    float decayRate;                // Decay rate
    SimulationStep lastStressStep;  // Time of last stress event
    float stressAccumulation;       // Accumulated stress
    
    Impl() : level(0.1f), locusCoeruleusGain(1.0f), stressLevel(0.0f),
             arousal(0.1f), vigilance(0.1f), baseline(0.05f), decayRate(0.05f),
             lastStressStep(0), stressAccumulation(0.0f) {}
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
    // NE modulates plasticity based on arousal
    return 0.5f + 0.5f * pImpl->arousal * (1.0f + pImpl->stressLevel);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal and vigilance
    pImpl->arousal = std::clamp(pImpl->level, 0.0f, 1.0f);
    pImpl->vigilance = 0.1f + 0.8f * pImpl->arousal;
    
    // Stress recovery
    if (pImpl->stressAccumulation > 0.0f) {
        pImpl->stressAccumulation = std::max(0.0f, pImpl->stressAccumulation - 0.001f * static_cast<float>(dt));
    }
}

void Norepinephrine::enhanceSensoryProcessing(float intensity, Brain* brain) {
    if (!brain || pImpl->arousal <= 0.0f) return;
    
    // Enhance sensory processing based on arousal
    float gain = 1.0f + 2.0f * pImpl->arousal * intensity;
    
    // Apply to sensory neurons - would need brain reference
    // This could enhance thalamocortical relay gain
}

void Norepinephrine::modulateARResponse(Brain* brain) {
    if (!brain || pImpl->vigilance <= 0.0f) return;
    
    // Enhance AR system responses
    if (brain->getAttention()) {
        // NE modulates attentional selection
    }
}

void Norepinephrine::signalStress(float stressMagnitude, float duration) {
    // Stress response - increases NE level and accumulates stress
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stressMagnitude);
    pImpl->stressAccumulation += stressMagnitude * duration;
    pImpl->level = std::min(1.0f, pImpl->level + stressMagnitude * 0.5f);
    pImpl->lastStressStep = 0;  // Would use brain reference
}

void Norepinephrine::enhanceAlertness(Brain* brain) {
    if (!brain || pImpl->vigilance <= 0.0f) return;
    
    // Enhance alertness and threat detection
    if (brain->getAttention()) {
        // NE enhances attentional focus on potential threats
    }
}

void Norepinephrine::modulateSTDPWindows(Brain* brain, Synapse* synapse) {
    if (!brain || !synapse || pImpl->arousal <= 0.0f) return;
    
    // NE modulates STDP windows based on arousal
    // Enhanced plasticity during high arousal states
    float arousalMod = 1.0f + pImpl->arousal;
    synapse->setLTPWeight(synapse->getLTPWeight() * arousalMod);
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

} // namespace nlm
