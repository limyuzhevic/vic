#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionStrength;
    float memoryRehearsalRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             attentionStrength(1.0f), memoryRehearsalRate(0.05f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances memory consolidation and attention
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Acetylcholine dynamics: respond to novel stimuli, decay with time
    // Baseline level promotes sustained attention
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Novel stimuli trigger acetylcholine release
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->attentionStrength);
}

void Acetylcholine::rehearseMemory(NeuronId neuron, float activation) {
    // ACh reinforces memory traces by enhancing synaptic efficacy
    if (activation > 0.1f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + activation * pImpl->memoryRehearsalRate);
    }
}

float Acetylcholine::getAttentionBoost() const {
    return pImpl->level;
}

} // namespace nlm