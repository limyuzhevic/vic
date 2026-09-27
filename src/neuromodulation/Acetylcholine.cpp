#include "Acetylcholine.hpp"
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
    float attentionSensitivity;
    float memoryEnhancement;
    float workingMemoryStrength;
    std::vector<float> attentionHistory;
    std::vector<float> memoryHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , attentionSensitivity(0.8f)
        , memoryEnhancement(0.6f)
        , workingMemoryStrength(0.7f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
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
    // Enhanced plasticity for memory-related regions
    // ACh modulates synaptic strengthening for attention-gated learning
    return 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionLevel) {
    // Increase ACh level based on attention demands
    if (attentionLevel > 0.0f) {
        float boost = attentionLevel * pImpl->attentionSensitivity;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
        pImpl->attentionHistory.push_back(pImpl->level);
    }
}

void Acetylcholine::signalLearning(float learningStrength) {
    // Enhance ACh during learning events
    if (learningStrength > 0.0f) {
        float enhancement = learningStrength * pImpl->memoryEnhancement;
        pImpl->level = std::min(pImpl->peak, pImpl->level + enhancement * pImpl->releaseRate);
        pImpl->memoryHistory.push_back(pImpl->level);
    }
}

void Acetylcholine::enhanceWorkingMemory(float memoryStrength) {
    // Directly enhance working memory encoding
    if (memoryStrength > 0.0f) {
        float boost = memoryStrength * pImpl->workingMemoryStrength;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    }
}

} // namespace nlm