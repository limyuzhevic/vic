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
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(0.6f)
        , arousalLevel(0.0f)
        , vigilanceLevel(0.0f) {}
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
    // NE modulates arousal and vigilance, affects plasticity
    // Optimal NE levels improve working memory and attention
    // Very high or very low NE impairs learning (inverted U-curve)
    return 1.0f / (1.0f + std::abs(pImpl->level - 0.5f) * 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float stimulusIntensity) {
    // NE responds to salient or novel stimuli
    pImpl->arousalLevel = std::clamp(stimulusIntensity, 0.0f, 1.0f);
    
    // Increase NE level
    if (stimulusIntensity > pImpl->level) {
        setLevel(std::min(pImpl->peak, pImpl->level + stimulusIntensity * pImpl->releaseRate * 0.8f));
    }
}

void Norepinephrine::signalVigilance(float focus) {
    // Sustained attention increases NE level
    pImpl->vigilanceLevel = std::clamp(focus, 0.0f, 1.0f);
    
    // For sustained vigilance, gradually increase NE
    if (focus > 0.0f) {
        setLevel(std::min(pImpl->peak, pImpl->level + focus * 0.1f * static_cast<float>(dt)));
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine (arousal/vigilance) system initialized");
}

} // namespace nlm