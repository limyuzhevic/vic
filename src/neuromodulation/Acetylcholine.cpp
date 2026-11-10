#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionStrength;
    float memoryConsolidationRate;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionStrength(1.0f)
        , memoryConsolidationRate(0.5f)
        , decayRate(0.05f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for important experiences
    return std::min(2.0f, 1.0f + pImpl->level * pImpl->memoryConsolidationRate);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay acetylcholine level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(const Observation& observation) {
    // Enhance attention to salient features in observation
    // Placeholder implementation: just increase level
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
}

void Acetylcholine::consolidateMemory(class Brain* brain) {
    // Enhance memory consolidation
    if (brain && pImpl->brain == brain) {
        // In a real implementation, this would affect neural plasticity
        pImpl->level = std::min(1.0f, pImpl->level + 0.05f);
    }
}

} // namespace nlm