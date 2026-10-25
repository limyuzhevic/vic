#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionWeight;
    float memoryConsolidationWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionWeight(0.7f)
        , memoryConsolidationWeight(0.8f)
        , decayRate(0.05f) {}
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
    // ACh enhances plasticity for attention-related processes
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh stabilizes attention and supports memory encoding
    // In a real implementation, this would affect neural excitability
    // and synaptic strengthening for attended information
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float salience) {
    // ACh amplifies saliency signals
    pImpl->level = std::min(1.0f, pImpl->level + salience * 0.3f);
}

void Acetylcholine::supportMemoryConsolidation() {
    // ACh promotes memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
}

} // namespace nlm