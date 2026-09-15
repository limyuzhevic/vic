#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalLevel;
    float vigilanceLevel;
    float baseline;
    float decayRate;
    float arousalBoostRate;
    float vigilanceBoostRate;
    float neuronExcitability;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , arousalLevel(0.5f)
        , vigilanceLevel(0.5f)
        , baseline(0.1f)
        , decayRate(0.05f)
        , arousalBoostRate(0.5f)
        , vigilanceBoostRate(0.5f)
        , neuronExcitability(1.0f) {}
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
    // NE modulates arousal-related plasticity
    // Higher NE increases plasticity for arousal-related circuits
    return 0.2f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Smooth arousal and vigilance based on level
    pImpl->arousalLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->vigilanceLevel = 0.4f + 0.6f * pImpl->level;
    
    // NE increases neuron excitability
    pImpl->neuronExcitability = 1.0f + 0.8f * pImpl->level;
}

void Norepinephrine::increaseArousal(float intensity) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + intensity * pImpl->arousalBoostRate);
    pImpl->level = std::min(1.0f, pImpl->level + intensity * 0.2f);
}

void Norepinephrine::regulateVigilance(float alertness) {
    pImpl->vigilanceLevel = std::clamp(alertness, 0.0f, 1.0f);
    pImpl->level = std::min(1.0f, pImpl->level + alertness * 0.3f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

} // namespace nlm