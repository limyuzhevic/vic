#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float noveltyThreshold;
    float attentionWeight;
    float memoryWeight;
    float decayRate;
    float sustainedActivation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , noveltyThreshold(0.3f)
        , attentionWeight(0.7f)
        , memoryWeight(0.6f)
        , decayRate(0.1f)
        , sustainedActivation(0.0f) {}
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
    // ACh enhances plasticity for memory encoding
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay toward baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Maintain sustained activation
    if (pImpl->sustainedActivation > 0.0f) {
        pImpl->level = std::min(1.0f, pImpl->level + pImpl->sustainedActivation * 0.01f);
        pImpl->sustainedActivation *= 0.95f;
    }
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

void Acetylcholine::modulateAttention(float attentionStrength) {
    // ACh directly modulates attentional selection
    setLevel(attentionStrength * pImpl->attentionWeight);
    
    // Enhance focus on salient stimuli
    if (pImpl->brain && pImpl->level > 0.5f) {
        // Would integrate with attentionalSelection system
        // For now, just log
        NLM_LOG_INFO("ACh modulating attention: level=" + std::to_string(pImpl->level));
    }
}

void Acetylcholine::enhanceMemoryEncoding(float noveltyLevel) {
    // ACh enhances memory encoding based on novelty
    if (noveltyLevel > pImpl->noveltyThreshold) {
        float enhancement = (noveltyLevel - pImpl->noveltyThreshold) / (1.0f - pImpl->noveltyThreshold);
        setLevel(std::min(1.0f, getLevel() + enhancement * pImpl->memoryWeight));
        
        NLM_LOG_INFO("ACh enhancing memory encoding: novelty=" + std::to_string(noveltyLevel) + 
                     " enhancement=" + std::to_string(enhancement));
    }
}

void Acetylcholine::maintainWorkingMemory(float activation) {
    // ACh maintains working memory traces
    if (activation > 0.1f) {
        setLevel(std::min(1.0f, getLevel() + activation * 0.2f));
    }
}

} // namespace nlm
