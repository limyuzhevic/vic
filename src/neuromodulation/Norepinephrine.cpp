#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousal;
    float vigilance;
    float stressLevel;
    float decayRate;
    float baseline;
    float peak;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousal(0.0f)
        , vigilance(0.0f)
        , stressLevel(0.0f)
        , decayRate(0.05f)
        , baseline(0.0f)
        , peak(1.0f) {}
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity during arousal
    return 0.5f + 0.5f * pImpl->arousal;
}

void Norepinephrine::update(TimestepDuration dt, float novelty, 
                          float predictionError, float reward) {
    // NE increases with stress, novelty, and high cognitive load
    float arousalDrive = 0.0f;
    
    if (predictionError != 0.0f) {
        // High prediction error increases arousal (cognitive challenge)
        arousalDrive += std::abs(predictionError) * 0.5f;
    }
    
    if (novelty != 0.0f) {
        // Novel stimuli increase arousal
        arousalDrive += novelty * 0.4f;
    }
    
    if (reward != 0.0f) {
        // Reward can either increase or decrease arousal depending on context
        arousalDrive += std::abs(reward) * 0.3f;
    }
    
    // Update arousal and vigilance
    pImpl->arousal = std::min(1.0f, pImpl->arousal + arousalDrive * 0.2f);
    pImpl->vigilance = 0.5f + pImpl->arousal * 0.5f;
    
    // Stress response from high arousal
    if (pImpl->arousal > 0.8f) {
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + 0.1f);
    }
    
    // Decay
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->arousal * 0.1f);
}

float Norepinephrine::getExcitabilityModulator() const {
    // NE increases neural excitability, especially in alertness
    return pImpl->level * 0.5f;
}

float Norepinephrine::getAttentionModulator() const {
    // NE enhances vigilance and focused attention
    return pImpl->vigilance;
}

float Norepinephrine::getMemoryModulator() const {
    // NE modulates memory encoding (enhanced during stress/arousal)
    return pImpl->stressLevel * 0.6f + pImpl->vigilance * 0.4f;
}

void Norepinephrine::increaseArousal(float strength) {
    // Boost arousal level
    pImpl->arousal = std::min(1.0f, pImpl->arousal + strength);
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * 0.5f);
}

void Norepinephrine::signalStress(float intensity) {
    // Stress response increases NE level
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + intensity);
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * 0.3f);
}

} // namespace nlm