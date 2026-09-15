#include "Neuromodulator.hpp"
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
    
    // Arousal and vigilance parameters
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    float labilityFactor;
    
    // NE dynamics
    float norepinephrineSynthesized;
    float norepinephrineReleased;
    float synthesisRate;
    float clearanceRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , arousalLevel(1.0f), vigilanceLevel(1.0f), stressLevel(0.0f), labilityFactor(0.5f)
        , norepinephrineSynthesized(0.0f), norepinephrineReleased(0.0f), synthesisRate(0.05f), clearanceRate(0.2f) {}
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
    // NE modulates arousal, vigilance, and stress response
    // Higher NE increases arousal and vigilance but may impair working memory at very high levels
    return 0.6f + 0.4f * pImpl->level * (1.0f - std::min(1.0f, pImpl->level / 0.7f));
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay towards baseline with stress adaptation
    float dt_float = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dt_float);
    
    // Clearance rate depends on current level
    pImpl->clearanceRate = 0.1f + 0.1f * pImpl->level;
    
    // Synthesize NE
    pImpl->norepinephrineSynthesized = pImpl->synthesisRate;
}

void Norepinephrine::signalArousal(float arousal) {
    // TODO PHASE 2: Implement arousal signaling
    // PLACEHOLDER: NE responds to arousal signals
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate);
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + arousal * 0.1f);
}

void Norepinephrine::signalStress(float stress) {
    // TODO PHASE 2: Implement stress response
    // PLACEHOLDER: Stress increases NE level with hysteresis
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stress * 0.1f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * pImpl->releaseRate * 2.0f);
}

void Norepinephrine::signalVigilance(float vigilance) {
    // TODO PHASE 2: Implement vigilance signaling
    // PLACEHOLDER: Vigilance increases NE level and improves attention
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + vigilance * 0.1f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilance * pImpl->releaseRate * 0.5f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 1.0f;
    pImpl->vigilanceLevel = 1.0f;
    pImpl->stressLevel = 0.0f;
    pImpl->norepinephrineSynthesized = 0.0f;
    pImpl->norepinephrineReleased = 0.0f;
}

} // namespace nlm