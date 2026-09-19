// Neuromodulation/Neuromodulator.cpp - Implementation of neuromodulator base class

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Neuromodulator::Neuromodulator() = default;

void Neuromodulator::update(TimestepDuration dt) {
    // Default implementation: decay towards baseline
    setLevel(getLevel() - decayRate_ * static_cast<float>(dt));
    if (getLevel() < 0.0f) {
        setLevel(0.0f);
    }
}

void Neuromodulator::configure(const Config& config) {
    // Default implementation: read configuration values
    if (config.has("neuromodulation_baseline")) {
        setBaseline(config.get<float>("neuromodulation_baseline"));
    }
    if (config.has("neuromodulation_decay_rate")) {
        setDecayRate(config.get<float>("neuromodulation_decay_rate"));
    }
}

void Neuromodulator::setBaseline(float baseline) {
    baseline_ = baseline;
}

float Neuromodulator::getBaseline() const {
    return baseline_;
}

void Neuromodulator::setDecayRate(float rate) {
    decayRate_ = rate;
}

float Neuromodulator::getDecayRate() const {
    return decayRate_;
}

} // namespace nlm
