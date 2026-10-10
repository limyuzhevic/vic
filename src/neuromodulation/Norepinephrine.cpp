#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(1.0f)
        , arousalLevel(0.0f)
        , vigilanceLevel(0.0f)
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
    // NE modulates arousal, vigilance, and stress
    float baseFactor = 0.5f + 0.5f * pImpl->level;
    
    // Apply arousal, vigilance, and stress effects
    float totalFactor = baseFactor * (1.0f + pImpl->arousalLevel + pImpl->vigilanceLevel + pImpl->stressLevel * 0.5f);
    return std::clamp(totalFactor, 0.2f, 2.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate arousal, vigilance, and stress based on level
    pImpl->arousalLevel = std::tanh(pImpl->level * 2.0f);  // Arousal increases with NE
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->level * 1.5f);  // Vigilance increases with NE
    pImpl->stressLevel = std::max(0.0f, (pImpl->level - 0.5f) * 2.0f);  // Stress increases when NE is high
}

void Norepinephrine::signalArousal(float arousalStrength) {
    // NE increases with arousal/stress
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalStrength * pImpl->releaseRate);
    NLM_LOG_INFO("Norepinephrine: Arousal signal increased level to " + std::to_string(pImpl->level));
}

void Norepinephrine::signalVigilance(float vigilanceStrength) {
    // NE increases with vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceStrength * pImpl->releaseRate * 0.8f);
    NLM_LOG_INFO("Norepinephrine: Vigilance signal increased level to " + std::to_string(pImpl->level));
}

void Norepinephrine::signalStress(float stressStrength) {
    // NE increases significantly with stress
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressStrength * pImpl->releaseRate * 2.0f);
    NLM_LOG_INFO("Norepinephrine: Stress signal increased level to " + std::to_string(pImpl->level));
}

} // namespace nlm
