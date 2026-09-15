// Neuromodulator system headers
#include "Neuromodulator.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;
    float memoryConsolidationFactor;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(0.5f),
             attentionModulation(1.0f), memoryConsolidationFactor(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

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
    // ACh enhances memory consolidation and attentional plasticity
    return 0.5f + 0.5f * pImpl->memoryConsolidationFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Attention signal boosts ACh level
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryEvent(float memoryStrength) {
    // Memory events enhance ACh level for consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * pImpl->releaseRate * 0.5f);
    pImpl->memoryConsolidationFactor = 1.2f;  // Boost memory consolidation
}

void Acetylcholine::updateAttentionModulation() {
    // Update attention modulation based on current level
    pImpl->attentionModulation = 1.0f + pImpl->level * 2.0f;
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

} // namespace nlm