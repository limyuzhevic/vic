#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float alertness;
    float workingMemoryEnhancement;
    float sensoryGain;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), alertness(0.5f), workingMemoryEnhancement(1.1f), 
             sensoryGain(1.0f), baseline(0.0f), decayRate(0.03f) {}
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()) {}

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
    // Norepinephrine enhances alertness and working memory
    return 1.0f + pImpl->workingMemoryEnhancement * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseAlertness(float intensity) {
    pImpl->alertness = std::clamp(intensity, 0.0f, 1.0f);
}

void Norepinephrine::enhanceWorkingMemory(float factor) {
    pImpl->workingMemoryEnhancement = std::max(1.0f, factor);
}

void Norepinephrine::modulateSensoryProcessing(float gain) {
    pImpl->sensoryGain = std::max(0.5f, gain);
}

} // namespace nlm