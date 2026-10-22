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
    float acetylcholineEffect;  // For memory enhancement
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , acetylcholineEffect(0.0f) {}
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
    // Update effect based on level
    pImpl->acetylcholineEffect = level * 0.8f;  // Strong memory enhancement effect
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity and memory consolidation
    // Higher levels increase both learning rate and memory stability
    return 0.5f + pImpl->level * 1.5f;  // Range: 0.5 to 2.0
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // If brain is available, update memory-related effects
    if (pImpl->brain && pImpl->brain->getWorkingMemory()) {
        // Modulate working memory strength based on acetylcholine level
        float memoryModulation = pImpl->level * 0.3f;
        
        // Apply to working memory
        if (memoryModulation > 0.0f) {
            // Enhance memory trace stability
            pImpl->brain->getWorkingMemory()->strengthenMemory(1.0f + memoryModulation * 2.0f);
        }
    }
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Signal attentional focus
    float newLevel = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate);
    setLevel(newLevel);
}

void Acetylcholine::signalLearning(float learningRate) {
    // Signal successful learning - enhance memory
    float noveltyBoost = learningRate * 0.3f;
    float newLevel = std::min(pImpl->peak, pImpl->level + noveltyBoost);
    setLevel(newLevel);
}

void Acetylcholine::signalMemoryRetrieval(float retrievalSuccess) {
    // Enhance memory traces on successful retrieval
    if (retrievalSuccess > 0.7f) {
        float retrievalBoost = (retrievalSuccess - 0.7f) * 0.5f;
        float newLevel = std::min(pImpl->peak, pImpl->level + retrievalBoost * 0.5f);
        setLevel(newLevel);
    }
}

float Acetylcholine::getMemoryEnhancementFactor() const {
    return pImpl->acetylcholineEffect;
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized - attention and memory enhancement");
}

} // namespace nlm
