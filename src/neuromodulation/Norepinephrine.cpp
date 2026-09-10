#pragma once

#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float noveltyWeight;
    float predictionErrorWeight;
    float arousalWeight;
    float focusWeight;
    float decayRate;
    float stressLevel;
    float alertness;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , noveltyWeight(0.6f)
        , predictionErrorWeight(0.4f)
        , arousalWeight(0.8f)
        , focusWeight(0.5f)
        , decayRate(0.05f)
        , stressLevel(0.0f)
        , alertness(0.0f) {}
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
    // NE modulates plasticity for arousal-based learning
    return 1.0f + pImpl->level * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics based on novelty and prediction error
    float noveltyDrive = pImpl->noveltyWeight * (pImpl->level - pImpl->baseline);
    float errorDrive = pImpl->predictionErrorWeight * (getLevel() - 0.5f);
    
    // Decay and integration
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt))
                  + noveltyDrive * 0.1f + errorDrive * 0.1f;
    
    // Stress modulation
    if (pImpl->stressLevel > 0.0f) {
        pImpl->level = std::min(1.0f, pImpl->level + pImpl->stressLevel * 0.2f);
        pImpl->stressLevel *= 0.9f;
    }
    
    // Update alertness
    pImpl->alertness = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

void Norepinephrine::modulateArousal(float novelty, float predictionError) {
    // NE increases arousal based on novelty and prediction error
    float noveltyBoost = novelty * pImpl->noveltyWeight;
    float errorBoost = std::abs(predictionError) * pImpl->predictionErrorWeight;
    
    setLevel(std::min(1.0f, getLevel() + noveltyBoost + errorBoost * 0.1f));
    
    NLM_LOG_INFO("NE modulating arousal: novelty=" + std::to_string(novelty) + 
                 " error=" + std::to_string(predictionError) + 
                 " newLevel=" + std::to_string(getLevel()));
}

void Norepinephrine::modulateAlertness(float sustainedAttention) {
    // NE maintains alertness for sustained attention
    setLevel(std::min(1.0f, getLevel() + sustainedAttention * pImpl->focusWeight * 0.05f));
    pImpl->alertness = sustainedAttention;
    
    NLM_LOG_INFO("NE modulating alertness: level=" + std::to_string(getLevel()) + 
                 " alertness=" + std::to_string(alertness));
}

void Norepinephrine::handleStress(float threatLevel) {
    // NE responds to threat/stress
    pImpl->stressLevel = std::clamp(threatLevel, 0.0f, 1.0f);
    setLevel(std::min(1.0f, getLevel() + threatLevel * 0.3f));
    
    NLM_LOG_INFO("NE handling stress: threat=" + std::to_string(threatLevel) + 
                 " level=" + std::to_string(getLevel()));
}

} // namespace nlm
