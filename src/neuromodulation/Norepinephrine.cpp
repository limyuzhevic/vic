#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalFactor;        // Norepinephrine's effect on arousal
    float attentionEnhancement; // Norepinephrine's effect on attention
    float stressModulation;     // Norepinephrine's effect on stress response
    float alertness;            // Current alertness level
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(2.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , arousalFactor(1.0f)
        , attentionEnhancement(1.0f)
        , stressModulation(0.0f)
        , alertness(0.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update derived effects based on norepinephrine level
    pImpl->arousalFactor = 1.0f + pImpl->level;
    pImpl->attentionEnhancement = 1.0f + pImpl->level * 0.5f;
    pImpl->stressModulation = std::min(1.0f, pImpl->level);
    pImpl->alertness = 0.5f + pImpl->level * 0.5f;
}

float Norepinephrine::getArousalFactor() const {
    return pImpl->arousalFactor;
}

float Norepinephrine::getAttentionEnhancement() const {
    return pImpl->attentionEnhancement;
}

float Norepinephrine::getStressModulation() const {
    return pImpl->stressModulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay toward baseline
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived effects
    pImpl->arousalFactor = 1.0f + pImpl->level;
    pImpl->attentionEnhancement = 1.0f + pImpl->level * 0.5f;
    pImpl->stressModulation = std::min(1.0f, pImpl->level);
    pImpl->alertness = 0.5f + pImpl->level * 0.5f;
}

void Norepinephrine::signalAlert(float alertness) {
    // Norepinephrine increases with alerts/stimuli
    pImpl->level = std::min(pImpl->peak,
                           pImpl->level + alertness * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stress) {
    // Norepinephrine increases with stress
    pImpl->level = std::min(pImpl->peak,
                           pImpl->level + stress * pImpl->releaseRate * 0.5f);
}

void Norepinephrine::signalRecovery() {
    // Norepinephrine decreases with recovery/rest
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - 0.5f * pImpl->releaseRate);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalFactor = 1.0f;
    pImpl->attentionEnhancement = 1.0f;
    pImpl->stressModulation = 0.0f;
    pImpl->alertness = 0.5f;
}

} // namespace nlm