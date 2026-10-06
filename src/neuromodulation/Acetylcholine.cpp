#include "Acetylcholine.hpp"
#include "brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionSignal;
    float memoryConsolidation;
    float plasticityInfluence;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(3.0f)
        , attentionSignal(0.0f)
        , memoryConsolidation(0.0f)
        , plasticityInfluence(1.0f) {}
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
    return pImpl->plasticityInfluence;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Attention modulation affects plasticity
    pImpl->plasticityInfluence = 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::enhanceAttention(float strength) {
    pImpl->attentionSignal = std::min(pImpl->peak, 
                                     pImpl->attentionSignal + strength * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryRequest(float priority) {
    if (priority > 0.5f) {
        pImpl->memoryConsolidation = std::min(pImpl->peak,
                                             pImpl->memoryConsolidation + priority * pImpl->releaseRate);
    }
}

void Acetylcholine::increaseFocus(float factor) {
    if (factor > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + factor * pImpl->releaseRate * 0.5f);
    }
}

void Acetylcholine::integrateWithLearning(float learningSignal, float confidence) {
    // Enhance memory consolidation during high confidence periods
    if (confidence > 0.7f) {
        pImpl->memoryConsolidation = std::min(pImpl->peak,
                                             pImpl->memoryConsolidation + learningSignal * 0.5f);
    }
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionSignal = 0.0f;
    pImpl->memoryConsolidation = 0.0f;
    pImpl->plasticityInfluence = 1.0f;
}

} // namespace nlm
