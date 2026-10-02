#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

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
        , memoryConsolidationWeight(0.3f)
        , decayRate(0.03f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine (ACh) system initialized");
}

const char* Acetylcholine::getName() const {
    return "Acetylcholine";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = level;
}

void Acetylcholine::update(float attention, float memory, TimestepDuration dt) {
    // ACh increases with attention and memory demand
    float target = pImpl->attentionWeight * attention + 
                   pImpl->memoryConsolidationWeight * memory;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances synaptic plasticity for memory consolidation
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::setAttentionWeight(float weight) {
    pImpl->attentionWeight = weight;
}

void Acetylcholine::setMemoryConsolidationWeight(float weight) {
    pImpl->memoryConsolidationWeight = weight;
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
