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
    float learningFactor;         // Acetylcholine's effect on learning
    float memoryEncoding;         // Acetylcholine's effect on memory encoding
    float corticalActivation;     // Acetylcholine's effect on cortical activation
    float attentionFocus;         // Acetylcholine's effect on attention focus
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(2.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , learningFactor(1.0f)
        , memoryEncoding(1.0f)
        , corticalActivation(1.0f)
        , attentionFocus(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update derived effects based on acetylcholine level
    pImpl->learningFactor = 1.0f + pImpl->level;
    pImpl->memoryEncoding = 1.0f + pImpl->level * 0.5f;
    pImpl->corticalActivation = 1.0f + pImpl->level * 0.3f;
    pImpl->attentionFocus = 1.0f + pImpl->level * 0.2f;
}

float Acetylcholine::getLearningFactor() const {
    return pImpl->learningFactor;
}

float Acetylcholine::getMemoryEncodingFactor() const {
    return pImpl->memoryEncoding;
}

float Acetylcholine::getCorticalActivation() const {
    return pImpl->corticalActivation;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay toward baseline
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived effects
    pImpl->learningFactor = 1.0f + pImpl->level;
    pImpl->memoryEncoding = 1.0f + pImpl->level * 0.5f;
    pImpl->corticalActivation = 1.0f + pImpl->level * 0.3f;
    pImpl->attentionFocus = 1.0f + pImpl->level * 0.2f;
}

void Acetylcholine::signalLearningEvent(float predictionError) {
    // Acetylcholine increases with learning prediction errors
    pImpl->level = std::min(pImpl->peak,
                           pImpl->level + predictionError * pImpl->releaseRate);
}

void Acetylcholine::signalAttentionDemand(float demand) {
    // Acetylcholine increases with attention demands
    pImpl->level = std::min(pImpl->peak,
                           pImpl->level + demand * pImpl->releaseRate * 0.5f);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->learningFactor = 1.0f;
    pImpl->memoryEncoding = 1.0f;
    pImpl->corticalActivation = 1.0f;
    pImpl->attentionFocus = 1.0f;
}

} // namespace nlm