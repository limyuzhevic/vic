#include "Norepinephrine.hpp"
#include "brain/Brain.hpp"
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
    float releaseRate;
    float arousalSignal;
    float vigilanceLevel;
    float plasticityInfluence;
    float locusCoeruleusInput;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(2.5f)
        , arousalSignal(0.0f)
        , vigilanceLevel(0.0f)
        , plasticityInfluence(1.0f)
        , locusCoeruleusInput(0.0f) {}
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
    return pImpl->plasticityInfluence;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Vigilance modulation affects plasticity
    pImpl->plasticityInfluence = 0.7f + 0.5f * pImpl->level;
}

void Norepinephrine::signalArousal(float intensity) {
    pImpl->arousalSignal = std::min(pImpl->peak, 
                                    pImpl->arousalSignal + intensity * pImpl->releaseRate);
}

void Norepinephrine::updateVigilance(float level) {
    pImpl->vigilanceLevel = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::enhanceAlertness(float factor) {
    if (factor > 0.0f) {
        pImpl->level = std::min(pImpl->peak, 
                                pImpl->level + factor * pImpl->releaseRate * 0.8f);
    }
}

void Norepinephrine::integrateWithAttention(float attentionLevel, float taskComplexity) {
    if (taskComplexity > 0.0f) {
        float additionalNE = attentionLevel * taskComplexity * 0.5f;
        pImpl->level = std::min(pImpl->peak, 
                                pImpl->level + additionalNE * pImpl->releaseRate * 0.3f);
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalSignal = 0.0f;
    pImpl->vigilanceLevel = 0.0f;
    pImpl->plasticityInfluence = 1.0f;
}

} // namespace nlm
