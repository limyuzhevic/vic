#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + pImpl->decayRate * static_cast<float>(dt));
    }
}

void Serotonin::modulateMood(float moodLevel) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * 0.3f);
}

void Serotonin::regulateImpulseControl(float controlStrength) {
    if (controlStrength > 0.3f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - controlStrength * 0.4f);
    }
}

void Serotonin::applyToNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Serotonin modulates mood and impulse control
    float moodMod = pImpl->level * 0.2f;
    neuron->injectCurrent(moodMod);
}

void Serotonin::applyToSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Serotonin reduces impulsive firing and modulates plasticity
    float plasticityMod = 1.0f - pImpl->level * 0.2f;
    synapse->setPlasticityFactor(synapse->getPlasticityFactor() * plasticityMod);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
}

float Serotonin::getBaseline() const {
    return pImpl->baseline;
}

void Serotonin::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

bool Serotonin::isActive() const {
    return pImpl->level > pImpl->baseline;
}

} // namespace nlm