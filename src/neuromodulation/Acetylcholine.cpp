#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float plasticityFactor;
    float attentionBoost;
    float memoryConsolidationRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , plasticityFactor(1.0f)
        , attentionBoost(0.0f)
        , memoryConsolidationRate(0.0f) {
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine (ACh)";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = level;
}

float Acetylcholine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh modulates attention and memory consolidation
    // Higher ACh enhances attention and stabilizes memory traces
    pImpl->attentionBoost = pImpl->level * 2.0f;  // Up to 2x attention boost
    pImpl->memoryConsolidationRate = pImpl->level * 0.1f;  // Up to 10x consolidation rate
    
    // Plasticity modulation (ACh enhances LTP)
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.3f;
    
    // Natural decay
    pImpl->level *= (1.0f - dt * 0.01f);
    
    NLM_LOG_INFO(std::string("Acetylcholine level: ") + std::to_string(pImpl->level) +
                 std::string(" | Attention boost: ") + std::to_string(pImpl->attentionBoost) +
                 std::string(" | Memory consolidation rate: ") + std::to_string(pImpl->memoryConsolidationRate));
}

} // namespace nlm
