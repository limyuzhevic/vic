#pragma once

#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float socialSensitivity;
    float rewardWeight;
    float inhibitionBaseline;
    float moodStabilization;
    float decayRate;
    float recoveryRate;
    float peakLevel;
    float socialCohesionBoost;
    float impulsivityReduction;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , socialSensitivity(1.0f)
        , rewardWeight(0.6f)
        , inhibitionBaseline(0.1f)
        , moodStabilization(0.5f)
        , decayRate(0.03f)
        , recoveryRate(0.15f)
        , peakLevel(1.0f)
        , socialCohesionBoost(1.5f)
        , impulsivityReduction(0.8f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::update(float socialEngagement, float rewardOutcome, TimestepDuration dt) {
    // 5-HT responds to social context and reward outcomes
    float socialResponse = socialEngagement * pImpl->socialSensitivity;
    float rewardResponse = rewardOutcome * pImpl->rewardWeight;
    
    // Strong increase for positive social outcomes
    if (socialEngagement > 0.6f && rewardOutcome > 0.7f) {
        // Social reward and positive feedback boost serotonin
        pImpl->level = std::min(pImpl->peakLevel, 
                              pImpl->level + socialResponse * 1.5f + rewardResponse * 0.5f);
    } else if (rewardOutcome > 0.5f) {
        // General positive feedback still helps
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + rewardResponse * 0.8f);
    }
    
    // Natural decay with mood stabilization
    float targetLevel = pImpl->inhibitionBaseline;
    if (socialEngagement > 0.3f) {
        targetLevel += socialResponse * 0.2f;
    }
    
    pImpl->level += (targetLevel - pImpl->level) * pImpl->moodStabilization;
    
    // Recovery from negative outcomes
    if (rewardOutcome < 0.2f) {
        pImpl->level = std::max(pImpl->inhibitionBaseline,
                              pImpl->level - pImpl->recoveryRate * static_cast<float>(dt) * 2.0f);
    }
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for social and emotional learning
    // Stable 5-HT promotes balanced, reflective learning
    float baseFactor = 1.0f;
    
    // Optimal range for learning: moderate 5-HT levels
    if (pImpl->level > 0.3f && pImpl->level < 0.8f) {
        baseFactor = 1.2f;  // Enhanced reflective learning
    } else if (pImpl->level > 0.8f) {
        baseFactor = 1.1f;  // Slightly reduced, but still functional
    }
    
    // Very low 5-HT impairs learning
    if (pImpl->level < 0.2f) {
        baseFactor = 0.9f;
    }
    
    return baseFactor;
}

void Serotonin::promoteSocialBehavior(NeuronId targetNeuron) {
    // 5-HT enhances social bonding and prosocial behaviors
    if (pImpl->brain) {
        pImpl->brain->injectCurrent(targetNeuron, 6.0f * pImpl->level);
    }
}

void Serotonin::regulateImpulsivity(float inhibitionTarget) {
    // 5-HT reduces impulsivity and promotes behavioral inhibition
    float inhibition = inhibitionTarget * pImpl->impulsivityReduction;
    if (pImpl->brain) {
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, inhibition * pImpl->level);
    }
}

void Serotonin::modulateDecisionMaking(float deliberationTime) {
    // 5-HT enhances decision-making stability and reduces rash choices
    if (pImpl->brain) {
        // Longer deliberation gets boost from 5-HT
        float boost = std::min(1.0f, deliberationTime * 0.1f) * pImpl->level;
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, boost * 4.0f);
    }
}

void Serotonin::enhanceWellbeing(float positiveFeedback) {
    // Positive feedback directly boosts serotonin
    setLevel(std::min(pImpl->peakLevel, getLevel() + positiveFeedback * 0.5f));
}

void Serotonin::setSocialSensitivity(float sensitivity) {
    pImpl->socialSensitivity = std::clamp(sensitivity, 0.0f, 3.0f);
}

void Serotonin::setRewardWeight(float weight) {
    pImpl->rewardWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Serotonin::setInhibitionBaseline(float baseline) {
    pImpl->inhibitionBaseline = std::clamp(baseline, 0.0f, 0.5f);
}

void Serotonin::setMoodStabilization(float stability) {
    pImpl->moodStabilization = std::clamp(stability, 0.0f, 1.0f);
}

float Serotonin::getSocialSensitivity() const {
    return pImpl->socialSensitivity;
}

float Serotonin::getRewardWeight() const {
    return pImpl->rewardWeight;
}

float Serotonin::getInhibitionBaseline() const {
    return pImpl->inhibitionBaseline;
}

float Serotonin::getMoodStabilization() const {
    return pImpl->moodStabilization;
}

void Serotonin::reset() {
    pImpl->level = pImpl->inhibitionBaseline;
}

} // namespace nlm