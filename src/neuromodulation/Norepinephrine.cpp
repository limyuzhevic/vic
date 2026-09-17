#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f),
             arousal(1.0f), vigilance(1.0f) {}
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
    // NE modulates arousal and vigilance
    // Affects learning rate and attention breadth
    float arousalMod = 1.0f + pImpl->level * 0.3f;
    
    return std::clamp(arousalMod, 0.5f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Medium decay rate
    pImpl->arousal = pImpl->arousal * 0.9f + 1.0f * 0.1f;
    pImpl->vigilance = pImpl->vigilance * 0.9f + 1.0f * 0.1f;
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Signal increased arousal (e.g., from novel stimuli)
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate * 1.3f);
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    // Signal increased vigilance (e.g., from threats)
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceLevel * pImpl->releaseRate);
}

} // namespace nlm