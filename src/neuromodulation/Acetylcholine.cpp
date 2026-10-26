#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), attentionGain(1.0f) {}
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
    // ACh enhances synaptic plasticity for attention-related learning
    // Higher ACh increases learning rate for attended stimuli
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays and modulates attention
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Enhance attention to specific neurons/signals
void Acetylcholine::enhanceAttention(NeuronId neuron, float gain) {
    // In a full implementation, this would modulate neural excitability
    // Higher ACh increases signal-to-noise ratio for attended inputs
}

} // namespace nlm
