#pragma once

#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionMod;
    float memoryConsolidation;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), releaseRate(1.0f), attentionMod(0.0f),
             memoryConsolidation(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized (Attention and Memory)");
}

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
    float attentionEffect = pImpl->attentionMod * 0.5f;
    float memoryEffect = pImpl->memoryConsolidation * 0.3f;
    return 0.5f + attentionEffect + memoryEffect;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    pImpl->attentionMod += (pImpl->level - pImpl->attentionMod) * 0.1f;
    pImpl->memoryConsolidation += pImpl->level * 0.05f;
}

void Acetylcholine::enhanceAttention(float stimulusStrength) {
    float attentionBoost = stimulusStrength * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionBoost);
}

void Acetylcholine::initiateMemoryConsolidation() {
    pImpl->memoryConsolidation = pImpl->level * 0.8f;
}

void Acetylcholine::modulateWorkingMemory() {
    if (pImpl->brain && pImpl->brain->getWorkingMemory()) {
        float modulation = pImpl->level * 0.3f;
    }
}

void Acetylcholine::signalAttentionShift() {
    if (pImpl->level > 0.5f) {
        pImpl->memoryConsolidation = pImpl->level * 0.3f;
    }
}

void Acetylcholine::decayMemoryTrace() {
    pImpl->memoryConsolidation *= 0.95f;
}

} // namespace nlm
