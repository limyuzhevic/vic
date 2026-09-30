#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    Brain* brain;
    float level;
    float plasticityFactor;
    float attentionGain;
    float memoryBoost;
    float noveltySensitivity;
    float decayRate;
    float learningRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , plasticityFactor(1.0f)
        , attentionGain(0.0f)
        , memoryBoost(0.0f)
        , noveltySensitivity(0.0f)
        , decayRate(0.05f)
        , learningRate(0.01f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

const char* Acetylcholine::getName() const {
    return "Acetylcholine (ACh)";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::max(0.0f, level);
}

float Acetylcholine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Apply decay to acetylcholine level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention gain based on current level
    pImpl->attentionGain = pImpl->level * 0.5f;
    
    // Update memory boost
    pImpl->memoryBoost = pImpl->level * 0.3f;
    
    // Update novelty sensitivity
    pImpl->noveltySensitivity = pImpl->level * 0.7f;
}

void Acetylcholine::signalAttention(float attention) {
    // Increase acetylcholine level based on attention demands
    pImpl->level += attention * pImpl->learningRate;
    
    // Update plasticity factor for memory consolidation
    pImpl->plasticityFactor = 1.0f + pImpl->memoryBoost * 0.5f;
    
    NLM_LOG_INFO("Acetylcholine: attention signal, level = " + std::to_string(pImpl->level));
}

void Acetylcholine::signalNovelty(float novelty) {
    // Increase acetylcholine level based on novelty
    pImpl->level += novelty * pImpl->learningRate * 2.0f;
    
    // Update novelty sensitivity
    pImpl->noveltySensitivity = std::min(1.0f, pImpl->noveltySensitivity + novelty * 0.1f);
    
    NLM_LOG_INFO("Acetylcholine: novelty signal, level = " + std::to_string(pImpl->level));
}

} // namespace nlm