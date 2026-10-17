#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalThreshold;
    float vigilanceWeight;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.03f)
        , arousalThreshold(0.3f)
        , vigilanceWeight(1.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized - arousal and vigilance");
}

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
    // Norepinephrine affects arousal and learning
    // Moderate levels optimize learning (inverted U-curve)
    if (pImpl->level < pImpl->arousalThreshold) {
        // Low arousal: suboptimal learning
        return 0.5f + 0.3f * pImpl->level;
    } else {
        // High arousal: enhanced but potential overload
        return 0.8f - 0.3f * (pImpl->level - pImpl->arousalThreshold);
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply vigilance effects on attention
    if (pImpl->brain && pImpl->level > 0.4f) {
        // Enhance attention when norepinephrine is high
        auto* attention = pImpl->brain->getAttention();
        if (attention) {
            // Note: This would require a method to boost attention levels
            // For now, we'll just log that vigilance is active
            NLM_LOG_INFO("High norepinephrine level enhancing vigilance");
        }
    }
}

void Norepinephrine::signal arousal(float arousalStrength) {
    // Bottom-up arousal signal
    setLevel(std::min(pImpl->peak, pImpl->level + arousalStrength * 0.4f));
}

void Norepinephrine::enhance Vigilance(float vigilanceLevel) {
    // Top-down vigilance signal
    setLevel(std::min(pImpl->peak, pImpl->level + vigilanceLevel * 0.3f));
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->level;
}

float Norepinephrine::getVigilanceBoost() const {
    return pImpl->vigilanceWeight;
}

void Norepinephrine::setArousalThreshold(float threshold) {
    pImpl->arousalThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Norepinephrine::setVigilanceWeight(float weight) {
    pImpl->vigilanceWeight = std::clamp(weight, 0.0f, 2.0f);
}

} // namespace nlm
