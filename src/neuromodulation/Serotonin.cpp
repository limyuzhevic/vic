#pragma once

#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float moodWeight;
    float impulsivityWeight;
    float socialWeight;
    float decayRate;
    float environmentalRiches;
    float moodState;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.3f)
        , moodWeight(0.7f)
        , impulsivityWeight(0.4f)
        , socialWeight(0.5f)
        , decayRate(0.08f)
        , environmentalRiches(0.0f)
        , moodState(0.5f) {}
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
    // Serotonin modulates plasticity for mood-dependent learning
    return 1.0f - pImpl->level * 0.2f;  // High serotonin reduces plasticity (more stable)
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics based on environmental factors
    float environmentalDrive = pImpl->environmentalRiches * pImpl->moodWeight;
    
    // Decay and integration
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt))
                  + environmentalDrive * 0.1f;
    
    // Update mood state
    pImpl->moodState = (pImpl->level - pImpl->baseline) / (1.0f - pImpl->baseline);
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

void Serotonin::modulateMood(float environmentalRiches) {
    // Serotonin responds to environmental richness
    pImpl->environmentalRiches = std::clamp(environmentalRiches, 0.0f, 1.0f);
    setLevel(std::min(1.0f, getLevel() + environmentalRiches * pImpl->moodWeight * 0.1f));
    
    NLM_LOG_INFO("5-HT modulating mood: riches=" + std::to_string(environmentalRiches) + 
                 " level=" + std::to_string(getLevel()) + 
                 " moodState=" + std::to_string(pImpl->moodState));
}

void Serotonin::modulateImpulsivity(float urgency) {
    // Serotonin reduces impulsivity
    float impulsivityReduction = urgency * pImpl->impulsivityWeight;
    setLevel(std::max(pImpl->baseline, getLevel() - impulsivityReduction * 0.1f));
    
    NLM_LOG_INFO("5-HT modulating impulsivity: urgency=" + std::to_string(urgency) + 
                 " level=" + std::to_string(getLevel()));
}

void Serotonin::modulateSocialBehavior(float socialContext) {
    // Serotonin enhances social behavior in positive contexts
    if (socialContext > 0.5f) {
        setLevel(std::min(1.0f, getLevel() + socialContext * pImpl->socialWeight * 0.05f));
    } else {
        // Negative social context reduces serotonin
        setLevel(std::max(pImpl->baseline, getLevel() - (1.0f - socialContext) * 0.05f));
    }
    
    NLM_LOG_INFO("5-HT modulating social behavior: context=" + std::to_string(socialContext) + 
                 " level=" + std::to_string(getLevel()));
}

} // namespace nlm
