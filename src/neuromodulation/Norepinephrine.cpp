#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Alertness level
    float alertness;
    
    // Stress response
    float stressLevel;
    
    // Vigilance level
    float vigilanceLevel;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , alertness(0.0f)
        , stressLevel(0.0f)
        , vigilanceLevel(0.0f) {}
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
    // NE effects: enhances alertness, vigilance, and stress responses
    // Different stress levels have different effects on plasticity
    float alertnessEffect = pImpl->alertness * 0.4f;
    float stressEffect = pImpl->stressLevel * 0.3f;
    float vigilanceEffect = pImpl->vigilanceLevel * 0.3f;
    
    // Stress can either enhance or impair plasticity depending on level
    if (pImpl->stressLevel > 0.7f) {
        return 0.5f + alertnessEffect + vigilanceEffect - 0.3f * (pImpl->stressLevel - 0.7f);
    }
    
    return 0.5f + alertnessEffect + vigilanceEffect + stressEffect * 0.2f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: decay towards baseline, respond to stress/arousal
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived effects
    pImpl->alertness = pImpl->level * 0.8f + 0.2f * pImpl->vigilanceLevel;
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->level * 0.5f + pImpl->vigilanceLevel * 0.5f);
}

float Norepinephrine::getAlertness() const {
    return pImpl->alertness;
}

void Norepinephrine::setAlertness(float alertness) {
    pImpl->alertness = std::clamp(alertness, 0.0f, 1.0f);
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::setStressLevel(float stress) {
    pImpl->stressLevel = std::clamp(stress, 0.0f, 1.0f);
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

void Norepinephrine::setVigilanceLevel(float vigilance) {
    pImpl->vigilanceLevel = std::clamp(vigilance, 0.0f, 1.0f);
}

} // namespace nlm
