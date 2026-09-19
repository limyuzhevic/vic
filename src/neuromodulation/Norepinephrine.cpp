// Neuromodulation/Norepinephrine.cpp - Implementation of norepinephrine neuromodulator

#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine (NE)";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine increases arousal and modulates plasticity
    return 0.5f + pImpl->level * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // Norepinephrine increases with arousal and stress
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate);
}

void Norepinephrine::signalAlertness(float alertness) {
    // Norepinephrine increases with alertness and focus
    pImpl->level = std::min(pImpl->peak, pImpl->level + alertness * pImpl->alertnessRate);
}

void Norepinephrine::configure(const Config& config) {
    if (config.has("norepinephrine_baseline")) {
        pImpl->baseline = config.get<float>("norepinephrine_baseline");
    }
    if (config.has("norepinephrine_peak")) {
        pImpl->peak = config.get<float>("norepinephrine_peak");
    }
    if (config.has("norepinephrine_decay_rate")) {
        pImpl->decayRate = config.get<float>("norepinephrine_decay_rate");
    }
    if (config.has("norepinephrine_release_rate")) {
        pImpl->releaseRate = config.get<float>("norepinephrine_release_rate");
    }
    if (config.has("norepinephrine_alertness_rate")) {
        pImpl->alertnessRate = config.get<float>("norepinephrine_alertness_rate");
    }
}

} // namespace nlm
