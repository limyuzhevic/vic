#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    float noveltySensitivity;
    float habituation;
    float phasicBurst;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.5f)  // Higher release rate for phasic responses
        , arousal(0.0f)
        , vigilance(0.0f)
        , noveltySensitivity(1.0f)
        , habituation(0.0f)
        , phasicBurst(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(class Brain* brain) {
    pImpl->brain = brain;
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
    // NE modulates plasticity for arousal-dependent learning
    float arousalFactor = std::clamp(pImpl->level, 0.2f, 1.0f) * 0.8f;  // Minimum 0.2 arousal for plasticity
    return 0.5f + arousalFactor * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply habituation to sustained NE
    if (pImpl->level > 0.3f) {
        pImpl->habituation += (pImpl->level - 0.3f) * 0.1f * static_cast<float>(dt);
        pImpl->habituation = std::min(1.0f, pImpl->habituation);
    } else {
        // Recover from habituation
        pImpl->habituation = std::max(0.0f, pImpl->habituation - 0.05f * static_cast<float>(dt));
    }
    
    // Update arousal and vigilance based on level and habituation
    float effectiveLevel = pImpl->level * (1.0f - pImpl->habituation * 0.5f);  // Habituation reduces effect
    pImpl->arousal = std::clamp(effectiveLevel, 0.0f, 1.0f);
    pImpl->vigilance = pImpl->arousal * 0.8f + pImpl->phasicBurst * 0.2f;
}

void Norepinephrine::increaseArousal(float amount) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
    // Reset habituation on phasic increase
    pImpl->habituation = std::max(0.0f, pImpl->habituation - 0.5f);
}

void Norepinephrine::enhanceVigilance(float amount) {
    pImpl->phasicBurst = std::min(1.0f, amount);
}

void Norepinephrine::processNovelty(float novelty, TimestepDuration dt) {
    if (novelty > 0.5f) {
        // Strong novelty triggers phasic NE release
        float burstAmount = (novelty - 0.5f) * 2.0f;
        pImpl->phasicBurst = std::max(pImpl->phasicBurst, burstAmount);
        
        // Phasic burst increases arousal and vigilance
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount * pImpl->releaseRate * 2.0f);
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + burstAmount * 0.5f);
    }
    
    // Sustained novelty increases general arousal
    if (novelty > 0.3f) {
        float sustainedAmount = (novelty - 0.3f) * 0.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + sustainedAmount * pImpl->releaseRate * 0.5f);
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousal = 0.0f;
    pImpl->vigilance = 0.0f;
    pImpl->noveltySensitivity = 1.0f;
    pImpl->habituation = 0.0f;
    pImpl->phasicBurst = 0.0f;
}

} // namespace nlm