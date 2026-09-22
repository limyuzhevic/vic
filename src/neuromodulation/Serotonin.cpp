#pragma once

#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;                    // Current serotonin level
    float mood;                     // Mood level (0-1)
    float inhibition;               // Behavioral inhibition level
    float impulsivity;              // Impulsivity level (inversely related to inhibition)
    float baseline;                 // Baseline level
    float decayRate;                // Decay rate
    SimulationStep lastRewardStep;  // Time since last reward
    float satiety;                  // Satiety level (0-1)
    float socialBonding;            // Social bonding level
    
    Impl() : level(0.5f), mood(0.5f), inhibition(0.5f), impulsivity(0.5f),
             baseline(0.3f), decayRate(0.05f), lastRewardStep(0),
             satiety(0.5f), socialBonding(0.5f) {}
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
    // Serotonin modulates plasticity based on mood and inhibition
    return 1.0f - 0.5f * pImpl->impulsivity * (1.0f - pImpl->inhibition);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood
    pImpl->mood = 0.3f + 0.7f * pImpl->level;
    
    // Update inhibition
    pImpl->inhibition = 0.2f + 0.8f * pImpl->level;
    
    // Update impulsivity (inverse of inhibition)
    pImpl->impulsivity = 1.0f - pImpl->inhibition;
    
    // Time-dependent mood changes
    float timeFactor = static_cast<float>(currentStep - pImpl->lastRewardStep) / 100.0f; // Would need brain reference
    if (timeFactor > 0.0f) {
        pImpl->mood = std::max(0.1f, pImpl->mood - timeFactor * 0.1f);
    }
}

void Serotonin::enhanceBehavioralInhibition(float contextFactor) {
    // Enhance behavioral inhibition for decision making
    float inhibitionBoost = 0.2f * pImpl->level * contextFactor;
    pImpl->inhibition = std::min(1.0f, pImpl->inhibition + inhibitionBoost);
    pImpl->impulsivity = 1.0f - pImpl->inhibition;
}

void Serotonin::modulateDecisionMaking(Brain* brain) {
    if (!brain || pImpl->inhibition <= 0.0f) return;
    
    // Enhance behavioral control and decision making
    if (brain->getAttention()) {
        // Serotonin enhances attentional focus and self-control
    }
}

void Serotonin::enhanceSelfControl(Brain* brain) {
    if (!brain || pImpl->inhibition <= 0.0f) return;
    
    // Enhance self-control mechanisms
    if (brain->getWorkingMemory() && brain->getEpisodicMemory()) {
        // Serotonin enhances long-term planning and self-regulation
    }
}

void Serotonin::signalReward(float reward, bool isExpected) {
    // Response to reward delivery
    if (isExpected) {
        // Expected reward: increase serotonin
        pImpl->level = std::min(1.0f, pImpl->level + reward * 0.1f);
        pImpl->satiety = std::min(1.0f, pImpl->satiety + reward * 0.05f);
    } else {
        // Unexpected reward: more dramatic response
        pImpl->level = std::min(1.0f, pImpl->level + reward * 0.2f);
    }
    
    pImpl->lastRewardStep = 0;  // Would use brain reference
}

void Serotonin::respondToSocialInteraction(float socialValue, Brain* brain) {
    if (!brain) return;
    
    // Serotonin responds to social rewards
    pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + socialValue * pImpl->level);
    
    if (brain->getWorkingMemory()) {
        // Encode social experiences
    }
}

void Serotonin::enhanceSocialBonding(float socialValue) {
    // Enhance social bonding based on serotonin level
    pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + socialValue * pImpl->level);
}

void Serotonin::modulateSocialLearning(Brain* brain) {
    if (!brain || pImpl->socialBonding <= 0.0f) return;
    
    // Enhance social learning based on serotonin
    if (brain->getWorkingMemory() && brain->getEpisodicMemory()) {
        // Encode social interactions for future imitation
    }
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

} // namespace nlm
