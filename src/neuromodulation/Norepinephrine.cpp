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
    float arousal;
    float vigilance;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f), arousal(1.0f), vigilance(1.0f) {}
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
    // NE modulates arousal and alertness
    // Higher NE increases synaptic efficacy and attention
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: increases arousal and alertness
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousalEvent() {
    // NE burst on high-priority events or threats
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.6f * pImpl->releaseRate);
}

void Norepinephrine::signalThreat() {
    // NE surge on perceived threat or challenge
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.8f * pImpl->releaseRate);
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

void Norepinephrine::setArousal(float arousal) {
    pImpl->arousal = std::clamp(arousal, 0.1f, 2.0f);
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

void Norepinephrine::setVigilance(float vigilance) {
    pImpl->vigilance = std::clamp(vigilance, 0.1f, 2.0f);
}

} // namespace nlm