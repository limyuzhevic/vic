#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousal;
    float vigilance;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), arousal(1.0f), vigilance(1.0f), 
             baseline(0.0f), decayRate(0.03f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE affects arousal and vigilance, which can enhance learning
    return 0.7f + 0.3f * pImpl->vigilance;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Gradual reduction of arousal and vigilance effects
    pImpl->arousal = std::max(1.0f, pImpl->arousal - 0.005f);
    pImpl->vigilance = std::max(1.0f, pImpl->vigilance - 0.01f);
}

void Norepinephrine::increaseArousal(float intensity) {
    pImpl->arousal = std::clamp(intensity, 1.0f, 3.0f);
    pImpl->level = std::min(1.0f, pImpl->level + intensity * 0.4f);
}

void Norepinephrine::enhanceVigilance(float focus) {
    pImpl->vigilance = std::clamp(focus, 1.0f, 3.0f);
    pImpl->level = std::min(1.0f, pImpl->level + focus * 0.3f);
}

} // namespace nlm
