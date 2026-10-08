#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryModulator;
    float attentionGain;
    float corticalActivator;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             memoryModulator(1.0f), attentionGain(1.0f), corticalActivator(1.0f) {}
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

float Acetylcholine::getMemoryModulator() const {
    // Acetylcholine modulates memory consolidation
    // Enhances memory formation for novel experiences
    return 1.0f + pImpl->level * 0.7f;
}

float Acetylcholine::getAttentionGain() const {
    // Acetylcholine enhances selective attention
    return 1.0f + pImpl->level * 0.6f;
}

float Acetylcholine::getCorticalActivator() const {
    // Acetylcholine activates cortical processing
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalNovelty(float novelty) {
    // Acetylcholine responds to novel stimuli and enhances encoding
    // Increases level for high novelty to promote learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 1.2f);
}

void Acetylcholine::signalAttention(float focus) {
    // Acetylcholine enhances attentional processing
    // Increases level for focused attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + focus * pImpl->releaseRate);
}

void Acetylcholine::signalMemory(float memoryLoad) {
    // Acetylcholine responds to memory processing demands
    pImpl->level = std::max(0.0f, pImpl->level + memoryLoad * pImpl->releaseRate * 0.4f);
}

float Acetylcholine::getLearningRateModifier() const {
    // Acetylcholine enhances learning rate for novel information
    return 1.0f + pImpl->level * 0.6f;
}

} // namespace nlm