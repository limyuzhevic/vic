#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

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

NeuromodulatorType Acetylcholine::getType() const {
    return NeuromodulatorType::Acetylcholine;
}

float Acetylcholine::getPlasticityFactor() const {
    return 0.5f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    float decay = 0.05f * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Acetylcholine::applyToNeuron(Neuron* neuron, float factor) {
    if (!neuron) return;
    neuron->injectCurrent(factor * pImpl->attentionBoost);
}

void Acetylcholine::applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) {
    ltpWeight *= pImpl->memoryGain;
    ltdWeight *= 1.1f;
}

void Acetylcholine::enhanceAttention() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.2f);
}

void Acetylcholine::facilitateMemoryEncoding() {}

void Acetylcholine::modulateSensoryProcessing(float gain) {
    pImpl->attentionBoost = 1.0f + gain;
}

} // namespace nlm
