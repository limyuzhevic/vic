#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + pImpl->decayRate * static_cast<float>(dt));
    }
}

void Norepinephrine::increaseArousal(float arousalLevel) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * 0.5f);
}

void Norepinephrine::triggerStressResponse(float stressLevel) {
    if (stressLevel > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * 0.8f);
    }
}

void Norepinephrine::applyToNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Norepinephrine increases alertness and firing threshold variability
    float arousalMod = pImpl->level * 0.4f;
    neuron->injectCurrent(arousalMod);
}

void Norepinephrine::applyToSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Norepinephrine modulates synaptic gain and plasticity
    float gainMod = 1.0f + pImpl->level * 0.3f;
    synapse->setGain(synapse->getGain() * gainMod);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
}

float Norepinephrine::getBaseline() const {
    return pImpl->baseline;
}

void Norepinephrine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

bool Norepinephrine::isActive() const {
    return pImpl->level > pImpl->baseline;
}

} // namespace nlm