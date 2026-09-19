#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

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
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances LTP and reduces LTD
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Acetylcholine increases with attention demands and memory load
    float attention = 0.0f;  // TODO: Get actual attention signal
    float memoryLoad = 0.0f;  // TODO: Get actual memory load
    
    // Acetylcholine increases with attention demands and memory load
    float target = pImpl->attentionStrength * attention + 
                   pImpl->memoryConsolidationRate * memoryLoad;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->level * pImpl->attentionStrength;
}

float Acetylcholine::getWorkingMemoryModulation() const {
    return pImpl->level * pImpl->memoryConsolidationRate;
}

void Acetylcholine::setAttentionStrength(float strength) {
    pImpl->attentionStrength = strength;
}

void Acetylcholine::setConsolidationRate(float rate) {
    pImpl->memoryConsolidationRate = rate;
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm