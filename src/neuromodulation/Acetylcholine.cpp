#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + pImpl->decayRate * static_cast<float>(dt));
    }
}

void Acetylcholine::enhanceAttention(float attentionStrength) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * 0.5f);
}

void Acetylcholine::promoteMemoryConsolidation() {
    if (pImpl->level > pImpl->baseline * 1.5f) {
        pImpl->level *= 1.2f;
    }
}

void Acetylcholine::applyToNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Acetylcholine enhances attention and learning readiness
    float attentionMod = pImpl->level * 0.3f;
    neuron->injectCurrent(attentionMod);
}

void Acetylcholine::applyToSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Acetylcholine enhances synaptic plasticity
    float plasticityBoost = 1.0f + pImpl->level * 0.5f;
    synapse->setPlasticityFactor(synapse->getPlasticityFactor() * plasticityBoost);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

float Acetylcholine::getBaseline() const {
    return pImpl->baseline;
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

bool Acetylcholine::isActive() const {
    return pImpl->level > pImpl->baseline;
}

} // namespace nlm