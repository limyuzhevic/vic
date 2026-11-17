#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionWeight;
    float memoryWeight;
    float baselineLevel;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionWeight(0.6f)
        , memoryWeight(0.4f)
        , baselineLevel(0.2f)
        , decayRate(0.02f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh level depends on attention demands and memory needs
    // In a full implementation, this would integrate with attention and memory systems
    // For now, use a simple baseline level with decay
    pImpl->level = pImpl->baselineLevel;
    
    // Decay toward baseline
    pImpl->level = pImpl->level * (1.0f - pImpl->decayRate * static_cast<float>(dt));
}

float Acetylcholine::getAttentionFactor() const {
    // Enhanced attention when ACh is high
    return std::pow(1.0f + pImpl->level * pImpl->attentionWeight, 2.0f);
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    // Better memory consolidation when ACh is high
    return std::pow(1.0f + pImpl->level * pImpl->memoryWeight, 1.5f);
}

void Acetylcholine::setAttentionWeight(float weight) {
    pImpl->attentionWeight = weight;
}

void Acetylcholine::setMemoryWeight(float weight) {
    pImpl->memoryWeight = weight;
}

void Acetylcholine::setBaselineLevel(float level) {
    pImpl->baselineLevel = level;
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm