#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionSignal;
    float memoryConsolidationSignal;
    float decayRate;
    float baseline;
    float peak;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionSignal(0.0f)
        , memoryConsolidationSignal(0.0f)
        , decayRate(0.1f)
        , baseline(0.0f)
        , peak(1.0f) {}
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-based learning
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt, float novelty, 
                          float predictionError, float reward) {
    // ACh increases with novel stimuli to enhance attention
    // Also promotes memory consolidation
    float attentionDrive = novelty * 0.7f + predictionError * 0.3f;
    
    // Smooth update of attention signal
    pImpl->attentionSignal += (attentionDrive - pImpl->attentionSignal) * 0.2f;
    pImpl->memoryConsolidationSignal = std::max(pImpl->memoryConsolidationSignal, 
                                                attentionDrive * 0.8f);
    
    // Decay ACh signals
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate level based on attention signal
    pImpl->level = std::min(pImpl->peak, 
                           pImpl->level + pImpl->attentionSignal * 0.1f);
}

float Acetylcholine::getExcitabilityModulator() const {
    // ACh moderately increases neural excitability
    return pImpl->level * 0.3f;
}

float Acetylcholine::getAttentionModulator() const {
    // ACh is the primary attention modulator
    return std::min(1.0f, pImpl->attentionSignal * 2.0f);
}

float Acetylcholine::getMemoryModulator() const {
    // ACh promotes memory consolidation
    return pImpl->memoryConsolidationSignal;
}

void Acetylcholine::enhanceAttention(float strength) {
    // Boost attention signal
    pImpl->attentionSignal = std::min(1.0f, pImpl->attentionSignal + strength);
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * 0.5f);
}

void Acetylcholine::promoteMemoryConsolidation(float strength) {
    // Promote memory consolidation
    pImpl->memoryConsolidationSignal = std::max(pImpl->memoryConsolidationSignal, strength);
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * 0.3f);
}

} // namespace nlm