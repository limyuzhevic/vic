#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float arousal;
    float vigilance;
    float focus;
    float responseStrength;
    float stress;
    float locusCoeruleusInput;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.05f), decayRate(0.03f),
             arousal(0.5f), vigilance(0.5f), focus(0.5f), responseStrength(0.5f),
             stress(0.1f), locusCoeruleusInput(0.5f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);  // NE can go up to 2.0
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine has an inverted U-shaped effect on memory and plasticity
    // Optimal at medium levels, high levels impair memory
    float normalizedLevel = pImpl->level / 2.0f;
    float invertedU = std::exp(-std::pow(normalizedLevel - 1.0f, 2.0f) * 4.0f);  // Peak at 1.0
    return 0.3f + 0.7f * invertedU;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Norepinephrine dynamics: quick rise, slow decay
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal-related functions
    pImpl->arousal = 0.3f + 0.7f * pImpl->level;
    pImpl->vigilance = 0.5f + 0.5f * pImpl->level;
    pImpl->focus = 0.4f + 0.6f * pImpl->level;
    pImpl->responseStrength = 0.3f + 0.7f * pImpl->level;
    pImpl->stress = 0.2f + 0.8f * std::min(1.0f, pImpl->level);
}

void Norepinephrine::signalAlert(float magnitude) {
    // Norepinephrine signals alertness and stress
    pImpl->level = std::min(2.0f, pImpl->level + magnitude * 0.8f);
}

void Norepinephrine::signalFocus(float magnitude) {
    // Norepinephrine can enhance focus and attention
    pImpl->level = std::min(2.0f, pImpl->level + magnitude * 0.5f);
}

void Norepinephrine::setArousalModifier(float modifier) {
    pImpl->arousal = std::clamp(modifier, 0.0f, 1.0f);
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getFocus() const {
    return pImpl->focus;
}

float Norepinephrine::getResponseStrength() const {
    return pImpl->responseStrength;
}

float Norepinephrine::getStress() const {
    return pImpl->stress;
}

void Norepinephrine::clearHistory() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm