#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilance;
    float learningRate;
    float decayRate;
    
    Impl() : level(0.0f), arousalLevel(0.5f), vigilance(1.0f), learningRate(1.2f), decayRate(0.08f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getPlasticityFactor() const {
    // NE increases learning rate and arousal
    return pImpl->learningRate * pImpl->arousalLevel;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Arousal dynamics
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Signal arousal for vigilance and learning
    pImpl->level = std::min(1.0f, arousalLevel);
    pImpl->arousalLevel = pImpl->level;
    pImpl->vigilance = 0.5f + 0.5f * pImpl->level;
}

} // namespace nlm
