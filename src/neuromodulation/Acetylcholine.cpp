#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionBias;
    float memoryBoost;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionBias(0.3f)
        , memoryBoost(0.5f)
        , decayRate(0.02f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACH";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates attention and memory consolidation
    // Higher ACh increases attention bias and memory strength
    float attentionMod = 1.0f + pImpl->attentionBias * pImpl->level;
    float memoryMod = 1.0f + pImpl->memoryBoost * pImpl->level;
    return attentionMod * memoryMod;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay back to baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getAttentionBias() const {
    return pImpl->attentionBias;
}

void Acetylcholine::setAttentionBias(float bias) {
    pImpl->attentionBias = bias;
}

float Acetylcholine::getMemoryBoost() const {
    return pImpl->memoryBoost;
}

void Acetylcholine::setMemoryBoost(float boost) {
    pImpl->memoryBoost = boost;
}

} // namespace nlm
