// Implementation of Acetylcholine neuromodulator
#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    // ACh effects on attention and memory
    pImpl->attentionMod = 1.2f;      // Increases attention
    pImpl->memoryMod = 1.5f;         // Enhances memory consolidation
    pImpl->plasticityMod = 1.1f;     // Modulates plasticity
    pImpl->arousalMod = 0.9f;        // Slightly decreases arousal
}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine neuromodulator initialized");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    NLM_LOG_DEBUG(std::string("ACh level set to: ") + std::to_string(pImpl->level));
}

float Acetylcholine::getAttentionEffect() const {
    return pImpl->attentionMod;
}

float Acetylcholine::getMemoryEffect() const {
    return pImpl->memoryMod;
}

float Acetylcholine::getPlasticityEffect() const {
    return pImpl->plasticityMod;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: increases with attention demands, decays naturally
    if (pImpl->brain) {
        // Modulate based on attention demands
        float attentionDemand = pImpl->brain->getAttentionDemand();
        pImpl->level += attentionDemand * 0.1f - pImpl->decayRate * static_cast<float>(dt);
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    if (pImpl->level > 0.5f) {
        NLM_LOG_DEBUG(std::string("ACh high: ") + std::to_string(pImpl->level));
    }
}

} // namespace nlm
