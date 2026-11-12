#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceLevel;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , arousalLevel(0.0f)
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
    // Norepinephrine modulates plasticity based on arousal
    // High arousal enhances plasticity for salient events
    // Low arousal reduces plasticity (conserves resources)
    float level = pImpl->level;
    float arousalFactor = pImpl->arousalLevel * 0.5f + 0.5f;
    return 0.3f + 0.7f * level * arousalFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real norepinephrine dynamics with realistic time constants
    static constexpr float timeConstant = 200.0f;  // 200 ms time constant
    float decayFactor = 1.0f - std::exp(-pImpl->decayRate * static_cast<float>(dt) / timeConstant);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

void Norepinephrine::triggerAlertness(float intensity) {
    // Transient alertness burst
    float alertnessBurst = std::min(intensity, 1.0f);
    setLevel(pImpl->level + alertnessBurst * 0.3f);
    
    // Update arousal and vigilance
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + alertnessBurst * 0.2f);
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + alertnessBurst * 0.1f);
    
    NLM_LOG_INFO("Norepinephrine-triggered alertness burst: " + std::to_string(alertnessBurst));
}

float Norepinephrine::getNELevel() const {
    return getLevel();
}

float Norepinephrine::getNEBaseline() const {
    return pImpl->baseline;
}

void Norepinephrine::setNEBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

} // namespace nlm
