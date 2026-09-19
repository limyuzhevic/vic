// Neuromodulation/Serotonin.cpp - Implementation of serotonin neuromodulator

#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin (5-HT)";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood and has complex effects on plasticity
    return 0.5f + pImpl->level * 0.4f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Serotonin increases with positive mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * pImpl->releaseRate);
}

void Serotonin::signalImpulse(float impulse) {
    // Serotonin influences impulsivity and behavioral control
    pImpl->level = std::min(pImpl->peak, pImpl->level + impulse * pImpl->impulseRate);
}

void Serotonin::configure(const Config& config) {
    if (config.has("serotonin_baseline")) {
        pImpl->baseline = config.get<float>("serotonin_baseline");
    }
    if (config.has("serotonin_peak")) {
        pImpl->peak = config.get<float>("serotonin_peak");
    }
    if (config.has("serotonin_decay_rate")) {
        pImpl->decayRate = config.get<float>("serotonin_decay_rate");
    }
    if (config.has("serotonin_release_rate")) {
        pImpl->releaseRate = config.get<float>("serotonin_release_rate");
    }
    if (config.has("serotonin_impulse_rate")) {
        pImpl->impulseRate = config.get<float>("serotonin_impulse_rate");
    }
}

} // namespace nlm
