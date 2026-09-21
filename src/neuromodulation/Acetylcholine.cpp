#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionalStrength;
    float memoryEnhancement;
    std::vector<float> attentionWeights;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.1f)
        , attentionalStrength(1.5f)
        , memoryEnhancement(1.2f) {}
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
    // Acetylcholine enhances plasticity for attention-related learning
    return 1.0f + pImpl->memoryEnhancement * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(bool focusedAttention, float predictedValue) {
    if (focusedAttention) {
        // Boost cholinergic activity for goal-directed attention
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * predictedValue);
    }
}

void Acetylcholine::enhanceMemory(NeuronId neuronId, float contextStrength) {
    // Enhance memory encoding and consolidation
    if (pImpl->level > 0.1f) {
        // Boost synaptic strengthening for attended information
        float enhancementFactor = 1.0f + pImpl->memoryEnhancement * pImpl->level;
        // Would apply to specific neurons in a real implementation
        // For now, just log
        if (pImpl->brain) {
            NLM_LOG_INFO("ACh enhancing memory encoding with factor " + std::to_string(enhancementFactor));
        }
    }
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm