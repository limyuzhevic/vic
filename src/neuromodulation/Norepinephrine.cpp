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
    float vigilanceFactor;
    float sensoryGain;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(0.8f),
             arousalLevel(1.0f), vigilanceFactor(1.0f), sensoryGain(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
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
    // NE modulates arousal and sensory processing
    return 0.7f + 0.3f * pImpl->sensoryGain;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalStrength) {
    // Arousal signals boost NE level
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalStrength * pImpl->releaseRate);
}

void Norepinephrine::signalVigilance(float vigilanceStrength) {
    // Vigilance signals boost NE level
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceStrength * pImpl->releaseRate * 0.8f);
    pImpl->vigilanceFactor = 1.3f;  // Enhance vigilance
}

float Norepinephrine::getSensoryGain() const {
    return pImpl->sensoryGain;
}

} // namespace nlm