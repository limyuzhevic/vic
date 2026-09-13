#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal state
    float alertness;
    float vigilance;
    float stressLevel;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.0f)
        , alertness(0.0f)
        , vigilance(0.0f)
        , stressLevel(0.0f) {}
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
    // NE modulates plasticity for arousal-related learning
    return 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real NE dynamics
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal components
    pImpl->alertness = std::max(0.0f, pImpl->alertness - pImpl->decayRate * 1.5f * static_cast<float>(dt));
    pImpl->vigilance = std::max(0.0f, pImpl->vigilance - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalAlertness(float level) {
    // Signal alertness/arousal
    pImpl->alertness = std::min(pImpl->peak, pImpl->alertness + level);
    pImpl->level = std::min(pImpl->peak, pImpl->level + level * 0.5f);
}

} // namespace nlm
