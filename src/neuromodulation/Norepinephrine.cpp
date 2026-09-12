#include "Neuromodulator.hpp"
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
    float arousalFactor;
    float vigilanceLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , arousalFactor(1.0f)
        , vigilanceLevel(0.5f) {}
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

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine (NE) system initialized - Arousal and vigilance");
}

void Norepinephrine::signalAlertness(float novelty, float predictionError) {
    // NE increases with novelty and prediction error (unexpected events)
    float alertnessSignal = (novelty * 0.6f) + (predictionError * 0.4f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + alertnessSignal * pImpl->releaseRate);
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + alertnessSignal * 0.1f);
}

void Norepinephrine::signalRecovery() {
    // NE decreases during rest/recovery
    float recoverySignal = 0.5f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - recoverySignal * pImpl->decayRate * 10.0f);
    pImpl->vigilanceLevel = std::max(0.1f, pImpl->vigilanceLevel - 0.05f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and enhances plasticity for salient events
    // Higher NE = better signal-to-noise ratio = enhanced learning for attended stimuli
    float arousalBoost = 1.0f + pImpl->level * 0.3f;  // Up to 1.3x normal plasticity
    float vigilanceBoost = 1.0f + pImpl->vigilanceLevel * 0.2f; // Up to 1.2x plasticity
    
    // NE particularly enhances plasticity for novel or surprising events
    return arousalBoost * vigilanceBoost;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Integrate with brain's attention system
    if (pImpl->brain && pImpl->brain->getAttention()) {
        // NE modulates attentional focus and vigilance
        float arousalLevel = pImpl->level;
        float vigilance = pImpl->vigilanceLevel;
        pImpl->brain->getAttention()->setNELevel(arousalLevel, vigilance);
    }
}

} // namespace nlm
