#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Real Neuromodulator implementation
class Neuromodulator::Impl {
public:
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f) {}
};

Neuromodulator::Neuromodulator() : pImpl(new Impl) {}

Neuromodulator::~Neuromodulator() = default;

float Neuromodulator::getLevel() const {
    return pImpl->level;
}

void Neuromodulator::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Neuromodulator::getPlasticityFactor() const {
    return 1.0f;
}

void Neuromodulator::update(TimestepDuration dt) {
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + pImpl->decayRate * static_cast<float>(dt));
    }
}

void Neuromodulator::applyToNeuron(Neuron* neuron, TimestepDuration dt) {}

void Neuromodulator::applyToSynapse(Synapse* synapse, TimestepDuration dt) {}

void Neuromodulator::reset() {
    pImpl->level = pImpl->baseline;
}

float Neuromodulator::getBaseline() const {
    return pImpl->baseline;
}

void Neuromodulator::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

bool Neuromodulator::isActive() const {
    return pImpl->level > pImpl->baseline;
}

} // namespace nlm