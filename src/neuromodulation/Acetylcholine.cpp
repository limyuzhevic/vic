#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionBoost;
    float memoryConsolidationBoost;
    float vigilanceLevel;
    
    Impl() : level(0.0f), attentionBoost(0.0f), memoryConsolidationBoost(0.0f), vigilanceLevel(0.0f) {}
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
    // ACh enhances plasticity, especially for memory-related processes
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Update ACh based on neural activity patterns
    // Simplified implementation: level increases with neural activity
    pImpl->level = 0.1f;  // Constant baseline for now
    
    // ACh boosts a temporary memory effects
    pImpl->memoryConsolidationBoost = pImpl->level * 0.8f;
    pImpl->attentionBoost = pImpl->level * 0.6f;
}

float Acetylcholine::getAttentionBoost() const {
    return pImpl->attentionBoost;
}

float Acetylcholine::getMemoryConsolidationBoost() const {
    return pImpl->memoryConsolidationBoost;
}

void Acetylcholine::signalAttention(float strength) {
    // This would be called when attention is directed to a stimulus
    setLevel(getLevel() + strength * 0.1f);
}

void Acetylcholine::signalMemory() {
    // This would be called during memory consolidation
    setLevel(getLevel() * 1.2f);
}

} // namespace nlm