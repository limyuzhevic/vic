#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float arousal;
    float vigilance;
    float decayRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), arousal(0.0f), vigilance(0.0f), decayRate(0.05f) {}
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
    // NE affects arousal and vigilance
    pImpl->arousal = level * 0.6f;
    pImpl->vigilance = level * 0.4f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE increases plasticity for arousal-related learning
    return 1.0f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousal = pImpl->level * 0.6f;
    pImpl->vigilance = pImpl->level * 0.4f;
}

void Norepinephrine::increaseArousal(float arousalLevel) {
    pImpl->level = std::min(1.0f, pImpl->level + arousalLevel);
    pImpl->arousal = std::min(1.0f, pImpl->arousal + arousalLevel * 0.6f);
}

void Norepinephrine::decreaseArousal(float arousalDecay) {
    pImpl->level = std::max(0.0f, pImpl->level - arousalDecay);
    pImpl->arousal = std::max(0.0f, pImpl->arousal - arousalDecay * 0.6f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousal;
}

} // namespace nlm