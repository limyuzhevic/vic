#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float neuralGain;
    float alertnessLevel;
    float responsiveness;
    float stressSensitivity;
    float anxietyLevel;
    float baselineArousal;
    float decayRate;
    float releaseRate;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , neuralGain(1.0f)
        , alertnessLevel(1.0f)
        , responsiveness(1.0f)
        , stressSensitivity(0.5f)
        , anxietyLevel(0.3f)
        , baselineArousal(0.2f)
        , decayRate(0.15f)
        , releaseRate(1.0f) {}
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
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity, enhancing learning under high arousal
    return 0.5f + 0.4f * pImpl->level + 0.1f * pImpl->stressSensitivity;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baselineArousal,
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate related functions based on current level
    pImpl->neuralGain = 1.0f + pImpl->level * 2.5f;
    pImpl->alertnessLevel = 1.0f + pImpl->level * 2.0f;
    pImpl->responsiveness = 1.0f + pImpl->level * 1.8f;
    pImpl->anxietyLevel = 0.3f + pImpl->level * 0.6f;
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Direct arousal signaling
    pImpl->level = std::min(1.0f, pImpl->level + arousalLevel * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stressLevel) {
    // Stress-induced norepinephrine release
    pImpl->level = std::min(1.0f, pImpl->level + stressLevel * pImpl->releaseRate * 1.5f);
    pImpl->stressSensitivity = std::min(1.0f, pImpl->stressSensitivity + stressLevel * 0.1f);
}

void Norepinephrine::signalAlertness(float alertness) {
    // Enhanced alertness signaling
    pImpl->level = std::min(1.0f, pImpl->level + alertness * pImpl->releaseRate * 0.8f);
}

float Norepinephrine::getNeuralGain() const {
    return pImpl->neuralGain;
}

float Norepinephrine::getAlertnessLevel() const {
    return pImpl->alertnessLevel;
}

float Norepinephrine::getResponsiveness() const {
    return pImpl->responsiveness;
}

float Norepinephrine::getStressSensitivity() const {
    return pImpl->stressSensitivity;
}

float Norepinephrine::getAnxietyLevel() const {
    return pImpl->anxietyLevel;
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->neuralGain = 1.0f;
    pImpl->alertnessLevel = 1.0f;
    pImpl->responsiveness = 1.0f;
    pImpl->stressSensitivity = 0.5f;
    pImpl->anxietyLevel = 0.3f;
}

} // namespace nlm