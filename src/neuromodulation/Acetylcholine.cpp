#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryEncoding;
    float learningRateMod;
    float noveltySensitivity;
    float corticalPlasticity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), attentionGain(1.0f), 
             memoryEncoding(0.5f), learningRateMod(1.0f), 
             noveltySensitivity(1.0f), corticalPlasticity(1.0f) {}
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
    // Acetylcholine modulates cortical plasticity
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Learning rate and encoding decay
    pImpl->learningRateMod = std::max(0.5f, pImpl->learningRateMod - pImpl->decayRate * static_cast<float>(dt));
    pImpl->memoryEncoding = std::max(0.1f, pImpl->memoryEncoding - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float focusStrength) {
    // Acetylcholine enhances attentional focus and selection
    pImpl->attentionGain = std::min(2.0f, pImpl->attentionGain + focusStrength * 0.8f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + focusStrength * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryEncoding(float encodingStrength) {
    // Acetylcholine promotes memory encoding and consolidation
    pImpl->memoryEncoding = std::min(1.0f, pImpl->memoryEncoding + encodingStrength * 0.9f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + encodingStrength * pImpl->releaseRate * 0.8f);
}

void Acetylcholine::promoteMemoryConsolidation() {
    // Acetylcholine promotes memory consolidation processes
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->releaseRate);
}

void Acetylcholine::enhanceLearningRate(float learningRateModulator) {
    // Acetylcholine enhances learning rate in novel situations
    pImpl->learningRateMod = std::min(2.0f, pImpl->learningRateMod + learningRateModulator * 0.7f);
    if (learningRateModulator > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + learningRateModulator * 0.2f);
    }
}

void Acetylcholine::integrateWithNoveltyDetection(float noveltySignal) {
    // Acetylcholine integrates with novelty detection
    if (noveltySignal > 0.0f) {
        pImpl->noveltySensitivity = std::min(1.5f, pImpl->noveltySensitivity + noveltySignal * 0.5f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + noveltySignal * 0.3f * pImpl->releaseRate);
    }
}

void Acetylcholine::modulateCorticalPlasticity(float plasticityRate) {
    // Acetylcholine modulates cortical plasticity rules
    pImpl->corticalPlasticity = std::min(2.0f, pImpl->corticalPlasticity + plasticityRate * 0.6f);
    if (plasticityRate > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + plasticityRate * 0.4f);
    }
}

} // namespace nlm