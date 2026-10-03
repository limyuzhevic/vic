#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float memoryConsolidationRate;
    float attentionModulation;
    float plasticityBoost;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , memoryConsolidationRate(1.0f)
        , attentionModulation(1.0f)
        , plasticityBoost(0.5f)
        , baseline(0.0f)
        , peak(1.0f)
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
    // Acetylcholine enhances synaptic consolidation and attention
    return 1.0f + pImpl->plasticityBoost * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Acetylcholine dynamics: increases during novel experiences, decays otherwise
    float target = pImpl->baseline + pImpl->level * 0.1f; // Reduced response to prevent runaway activation
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.05f;
    
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalNovelExperience(float noveltyScore) {
    // High ACh during novel experiences signals "pay attention"
    float burst = noveltyScore * 0.8f;
    setLevel(std::min(pImpl->peak, getLevel() + burst));
}

void Acetylcholine::triggerMemoryConsolidation() {
    // ACh triggers memory consolidation during learning
    float consolidationBoost = 0.5f * getLevel();
    // In a real implementation, this would affect memory systems
    // For now, just store the consolidation signal
    pImpl->memoryConsolidationRate = 1.0f + consolidationBoost;
}

void Acetylcholine::enhanceAttention() {
    // ACh modulates attention mechanisms
    pImpl->attentionModulation = 1.0f + getLevel() * 0.5f;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionModulation = 1.0f;
    pImpl->plasticityBoost = 0.5f;
}

} // namespace nlm
