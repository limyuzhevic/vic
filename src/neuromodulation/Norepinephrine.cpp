#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float stressSensitivity;
    float rewardSensitivity;
    float noveltyResponse;
    
    // Alertness and vigilance state
    float alertnessLevel;
    float vigilanceFocus;
    float responsePriority;
    
    // Arousal dynamics
    float currentArousal;
    float activationThreshold;
    float habituationRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.15f)
        , stressSensitivity(0.6f)
        , rewardSensitivity(0.2f)
        , noveltyResponse(0.3f)
        , alertnessLevel(0.5f)
        , vigilanceFocus(0.5f)
        , responsePriority(0.5f)
        , currentArousal(0.5f)
        , activationThreshold(0.3f)
        , habituationRate(0.1f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getAlertness() const {
    // Higher NE improves alertness and focus
    return 0.3f + pImpl->level * 0.7f;
}

float Norepinephrine::getAttentionPriority() const {
    // NE prioritizes important/salient stimuli
    return 0.4f + pImpl->level * 0.6f;
}

float Norepinephrine::getArousalLevel() const {
    // NE directly influences arousal
    return pImpl->currentArousal;
}

void Norepinephrine::update(TimestepDuration dt, float stress, float novelty, float importance) {
    pImpl->currentArousal = pImpl->level;
    
    // Stress challenge drives NE release
    float stressDriven = pImpl->stressSensitivity * stress;
    
    // Novelty generates NE response
    float noveltyDriven = pImpl->noveltyResponse * novelty;
    
    // Importance/positive outcomes can enhance NE
    float rewardDriven = pImpl->rewardSensitivity * importance;
    
    // Combined input
    float input = stressDriven + noveltyDriven + rewardDriven;
    
    // Smooth update
    pImpl->level += (input - pImpl->level) * 0.2f;
    
    // Decay with habituation
    float decay = pImpl->decayRate * static_cast<float>(dt);
    float habituation = pImpl->habituationRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay - habituation);
    
    // Update alertness
    pImpl->alertnessLevel = 0.2f + pImpl->level * 0.8f;
    
    // Update vigilance focus
    pImpl->vigilanceFocus = 0.3f + pImpl->level * 0.7f;
    
    // Response priority increases with NE
    pImpl->responsePriority = 0.2f + pImpl->level * 0.8f;
}

void Norepinephrine::signalChallenge(float intensity) {
    // Challenge/stress increases NE
    if (intensity > pImpl->activationThreshold) {
        float boost = intensity * pImpl->stressSensitivity;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * 0.4f);
    }
}

void Norepinephrine::signalReward(float value) {
    // Reward/positive outcome can modulate NE
    if (value > 0.5f) {
        float enhancement = value * pImpl->rewardSensitivity;
        pImpl->level = std::min(pImpl->peak, pImpl->level + enhancement * 0.3f);
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->alertnessLevel = 0.5f;
    pImpl->vigilanceFocus = 0.5f;
    pImpl->responsePriority = 0.5f;
    pImpl->currentArousal = 0.5f;
}

} // namespace nlm