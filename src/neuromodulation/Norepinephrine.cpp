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
    float arousalSensitivity;
    float vigilanceLevel;
    float stressResponse;
    std::vector<float> arousalHistory;
    std::vector<float> stressHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.02f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.5f)
        , arousalSensitivity(0.9f)
        , vigilanceLevel(0.7f)
        , stressResponse(0.8f) {}
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
    // NE enhances plasticity for arousal-related learning
    // Higher NE increases synaptic gain and attention to salient stimuli
    return 0.4f + 0.6f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalStress(float stressLevel) {
    // Increase NE level based on stress
    if (stressLevel > 0.0f) {
        float response = stressLevel * pImpl->stressResponse;
        pImpl->level = std::min(pImpl->peak, pImpl->level + response * pImpl->releaseRate);
        pImpl->stressHistory.push_back(pImpl->level);
    }
}

void Norepinephrine::increaseVigilance(float vigilanceLevel) {
    // Enhance vigilance and alertness
    if (vigilanceLevel > 0.0f) {
        float boost = vigilanceLevel * pImpl->vigilanceLevel;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    }
}

void Norepinephrine::enhanceAlertness(float alertnessStrength) {
    // Direct enhancement of neural alertness
    if (alertnessStrength > 0.0f) {
        float boost = alertnessStrength * pImpl->arousalSensitivity;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    }
}

} // namespace nlm