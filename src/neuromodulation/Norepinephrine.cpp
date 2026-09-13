#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalBoost;
    float vigilanceBoost;
    float stressResponse;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f),
             arousalBoost(1.5f), vigilanceBoost(1.2f), stressResponse(0.8f) {}
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
    // NE enhances alertness-related plasticity
    // Boosts encoding of salient/novel information
    return 1.0f + pImpl->arousalBoost * pImpl->level + pImpl->vigilanceBoost * pImpl->level * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalStress(float stress) {
    // NE increases with stress (threat/arousal)
    // Also modulates locus coerulean firing
    if (stress > 0.0f) {
        setLevel(std::min(pImpl->level + stress * pImpl->stressResponse * pImpl->releaseRate, pImpl->peak));
    }
}

void Norepinephrine::signalLocusCoeruleActivation(float activation) {
    // Direct LC firing affects NE release
    // Phasic responses to novel events
    setLevel(pImpl->level + activation * 0.5f);
}

void Norepinephrine::signalArousal(float arousal) {
    // Tonic arousal signals
    if (arousal > pImpl->level) {
        setLevel(arousal);
    }
}

} // namespace nlm
