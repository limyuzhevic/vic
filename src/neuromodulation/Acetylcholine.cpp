#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float consolidationRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), attentionGain(1.5f), consolidationRate(0.8f) {}
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
    // Acetylcholine enhances synaptic consolidation and attention
    // Higher ACh increases plasticity for attended/relevant information
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline but boost on relevant inputs
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Attention signaling - ACh increases during novel or relevant stimuli
void Acetylcholine::signalAttention(float novelty, float relevance) {
    // Boost level based on novelty and relevance
    float boost = (novelty + relevance) * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
}

// Memory consolidation - ACh enhances long-term potentiation
void Acetylcholine::signalConsolidation(float memoryStrength) {
    // Store memory more effectively
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * pImpl->consolidationRate);
}

} // namespace nlm