#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodWeight;
    float impulsivityWeight;
    float socialBehaviorWeight;
    float rewardSensitivity;
    float satietyLevel;
    float baselineLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , moodWeight(0.7f)
        , impulsivityWeight(0.3f)
        , socialBehaviorWeight(0.6f)
        , rewardSensitivity(0.5f)
        , satietyLevel(0.2f)
        , baselineLevel(0.1f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    // Serotonin has complex effects on behavior
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update satiety: higher serotonin means feeling "full"
    if (pImpl->level > 0.5f) {
        pImpl->satietyLevel = std::min(1.0f, pImpl->satietyLevel + 0.1f);
    } else {
        pImpl->satietyLevel = std::max(0.0f, pImpl->satietyLevel - 0.05f);
    }
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects learning in several ways:
    // - Increases reward prediction accuracy
    // - Reduces impulsivity in decision making
    // - Enhances persistence for long-term rewards
    
    float factor = 1.0f;
    
    // Mood modulation: better mood improves learning
    factor += pImpl->level * pImpl->moodWeight;
    
    // Impulsivity reduction: higher serotonin = more patient learning
    if (pImpl->level > 0.3f) {
        factor += (pImpl->level - 0.3f) * (1.0f - pImpl->impulsivityWeight);
    }
    
    // Reward sensitivity: modulates response to rewards
    factor *= (1.0f + pImpl->rewardSensitivity * pImpl->level);
    
    return factor;
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(pImpl->baselineLevel, pImpl->level - 0.02f * static_cast<float>(dt));
}

void Serotonin::signalReward(float reward, float intensity) {
    // Responds to reward, especially unexpected rewards
    // Higher intensity rewards cause larger serotonin bursts
    float response = reward * intensity * 0.3f;
    
    setLevel(pImpl->level + response);
}

void Serotonin::signalSocialFeedback(float socialValue, bool positive) {
    // Social behavior modulation
    float response = socialValue * (positive ? 0.4f : -0.2f);
    setLevel(pImpl->level + response);
}

float Serotonin::getMood() const {
    // Simple mood model based on serotonin level
    return pImpl->level * 2.0f - 1.0f;  // Maps to -1.0 to +1.0
}

float Serotonin::getImpulsivity() const {
    // Higher serotonin reduces impulsivity
    return std::max(0.0f, 1.0f - pImpl->level * pImpl->impulsivityWeight);
}

float Serotonin::getSatiety() const {
    return pImpl->satietyLevel;
}

} // namespace nlm