/*
 * Definition file for neuromodulators (Serotonin, Norepinephrine, Acetylcholine)
 * Provides the actual implementation classes that integrate with the base Neuromodulator interface
 */

#ifndef __NEUROMODULATORS_HPP
#define __NEUROMODULATORS_HPP

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float plasticityFactor;
    
    // Social behavior state
    float socialCohesion;
    float impulsivity;
    float mood;
    
    // Memory effects
    float memoryConsolidation;
    SimulationStep lastConsolidation;
    
    Impl() : level(0.1f), baseline(0.1f), plasticityFactor(1.0f),
             socialCohesion(0.5f), impulsivity(0.5f), mood(0.5f),
             memoryConsolidation(1.0f), lastConsolidation(0) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const { return "Serotonin"; }

float Serotonin::getLevel() const { return pImpl->level; }

void Serotonin::setLevel(float level) { 
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const { 
    // Serotonin modulates plasticity - affects learning rate
    // Low serotonin = high impulsivity = faster but less stable learning
    // High serotonin = better mood = slower but more stable learning
    float basePlasticity = 1.0f - pImpl->level;  // Less plasticity with higher serotonin
    return std::max(0.1f, basePlasticity + pImpl->impulsivity * 0.3f);
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level *= (1.0f - dt * 0.03f);
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Social cohesion decays
    pImpl->socialCohesion *= (1.0f - dt * 0.02f);
    
    // Mood drifts slowly
    pImpl->mood += (0.5f - pImpl->mood) * dt * 0.01f;
}

void Serotonin::signalMoodChange(float mood) {
    pImpl->mood = std::clamp(mood, 0.0f, 1.0f);
    
    // Mood affects serotonin levels
    if (mood > 0.7f) {
        // Positive mood increases serotonin
        setLevel(std::min(1.0f, getLevel() + 0.2f));
        pImpl->socialCohesion += 0.1f;
    } else if (mood < 0.3f) {
        // Negative mood decreases serotonin
        setLevel(std::max(0.0f, getLevel() - 0.1f));
        pImpl->socialCohesion *= 0.9f;
    }
}

void Serotonin::signalSocialInteraction(float socialReward) {
    // Social interaction increases serotonin
    setLevel(std::min(1.0f, getLevel() + socialReward * 0.3f));
    pImpl->socialCohesion += socialReward * 0.1f;
    pImpl->impulsivity *= (1.0f - socialReward * 0.05f);
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float plasticityFactor;
    
    // Arousal state
    float arousal;
    float vigilance;
    float attention;
    
    // Novelty detection
    float noveltySensitivity;
    std::vector<float> recentInputs;
    
    // Stress/panic state
    float stressLevel;
    float panicThreshold;
    
    Impl() : level(0.2f), baseline(0.1f), plasticityFactor(1.0f),
             arousal(0.5f), vigilance(0.5f), attention(0.5f),
             noveltySensitivity(0.5f), stressLevel(0.0f), panicThreshold(0.8f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const { return "Norepinephrine"; }

float Norepinephrine::getLevel() const { return pImpl->level; }

void Norepinephrine::setLevel(float level) { 
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const { 
    // Norepinephrine affects plasticity and memory encoding
    // High NE = high arousal = enhanced plasticity for salient events
    float arousalBoost = std::pow(pImpl->arousal, 1.5f);  // Nonlinear effect
    return std::min(3.0f, 1.0f + arousalBoost * (pImpl->level - 0.5f) * 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level *= (1.0f - dt * 0.04f);
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Arousal drifts
    pImpl->arousal += (0.5f - pImpl->arousal) * dt * 0.02f;
    pImpl->vigilance += (0.5f - pImpl->vigilance) * dt * 0.01f;
    
    // Novelty sensitivity adaptation
    pImpl->noveltySensitivity *= (1.0f - dt * 0.01f);
}

void Norepinephrine::signalArousal(float arousalLevel) {
    pImpl->arousal = std::clamp(arousalLevel, 0.0f, 1.0f);
    
    // Arousal affects NE levels
    if (arousalLevel > 0.8f) {
        // High arousal triggers NE release
        setLevel(std::min(1.0f, getLevel() + 0.3f));
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.2f);
    }
}

void Norepinephrine::signalNovelty(float noveltyLevel) {
    pImpl->noveltySensitivity = std::min(1.0f, pImpl->noveltySensitivity + noveltyLevel * 0.1f);
    
    // Novelty affects NE
    if (noveltyLevel > 0.7f) {
        // Strong novelty triggers NE response
        setLevel(std::min(1.0f, getLevel() + noveltyLevel * 0.2f));
    }
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float plasticityFactor;
    
    // Attention state
    float attentionStrength;
    float acetylcholineProduction;
    
    // Memory consolidation
    float consolidationRate;
    SimulationStep lastConsolidationTime;
    std::vector<float> recentMemories;
    
    // Behavioral state
    float explorationInhibition;
    float rewardModulation;
    
    Impl() : level(0.2f), baseline(0.1f), plasticityFactor(1.0f),
             attentionStrength(0.5f), acetylcholineProduction(0.0f),
             consolidationRate(1.0f), lastConsolidationTime(0),
             explorationInhibition(0.5f), rewardModulation(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const { return "Acetylcholine"; }

float Acetylcholine::getLevel() const { return pImpl->level; }

void Acetylcholine::setLevel(float level) { 
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const { 
    // ACh affects attention and memory consolidation
    // High ACh = focused attention = enhanced plasticity for attended stimuli
    return std::min(2.5f, pImpl->plasticityFactor * (1.0f + pImpl->attentionStrength * 1.5f));
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level *= (1.0f - dt * 0.03f);
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Attention dynamics
    pImpl->attentionStrength *= (1.0f - dt * 0.05f);
    
    // Memory consolidation rate varies
    pImpl->consolidationRate = 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::signalAttention(float attentionStrength) {
    pImpl->attentionStrength = std::clamp(attentionStrength, 0.0f, 1.0f);
    
    // Attention affects ACh production
    if (attentionStrength > 0.7f) {
        // Focused attention triggers ACh release
        setLevel(std::min(1.0f, getLevel() + 0.2f));
        pImpl->explorationInhibition = std::max(0.1f, pImpl->explorationInhibition - 0.1f);
    }
}

void Acetylcholine::signalMemoryConsolidation(float consolidationStrength) {
    pImpl->lastConsolidationTime = 0;  // Reset consolidation timer
    
    // Consolidation affects ACh
    setLevel(std::min(1.0f, getLevel() + consolidationStrength * 0.1f));
    pImpl->consolidationRate += consolidationStrength * 0.2f;
}

} // namespace nlm

#endif // __NEUROMODULATORS_HPP