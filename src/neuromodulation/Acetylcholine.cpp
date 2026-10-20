#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryConsolidationRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(0.8f),
             attentionGain(1.0f), memoryConsolidationRate(0.5f) {}
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
    // ACh reduces plasticity to stabilize learning and promote memory consolidation
    return 1.0f - pImpl->level * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline, influenced by attention demands
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionShift(float priority) {
    // ACh signals important information for attention allocation
    if (priority > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + priority * pImpl->releaseRate);
    }
}

void Acetylcholine::signalMemoryConsolidation(float consolidationSignal) {
    // ACh promotes memory consolidation, especially during rest
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationSignal * pImpl->releaseRate * 0.5f);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain * pImpl->level;
}

float Acetylcholine::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate + pImpl->level * 0.5f;
}

} // namespace nlm
