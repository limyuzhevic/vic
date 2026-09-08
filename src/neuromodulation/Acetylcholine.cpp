#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float modulationStrength;
    float memoryConsolidationBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , modulationStrength(1.0f)
        , memoryConsolidationBoost(1.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized (attention and memory)");
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decayRate = 0.05f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayRate * static_cast<float>(dt));
}

void Acetylcholine::boostAttention(float strength, NeuronId neuron) {
    // Implementation would modulate neural excitability
    // For now, just log the effect
    NLM_LOG_INFO("Acetylcholine boosting attention by " + std::to_string(strength));
}

void Acetylcholine::enhanceMemoryConsolidation(float duration) {
    // Implementation would boost memory consolidation
    // For now, just log the effect
    NLM_LOG_INFO("Acetylcholine enhancing memory consolidation for " + std::to_string(duration) + "s");
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
    // Acetylcholine increases plasticity for memory consolidation
    return 1.0f + pImpl->level * 0.5f;
}

} // namespace nlm
