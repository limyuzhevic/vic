#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float consolidationBoost;
    float plasticityBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , attentionBoost(0.0f)
        , consolidationBoost(0.0f)
        , plasticityBoost(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(class Brain* brain) {
    pImpl->brain = brain;
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
    // ACh modulates plasticity for enhanced learning
    float plasticityFactor = 1.0f + pImpl->level * 0.5f;
    return plasticityFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention and consolidation effects
    if (pImpl->level > 0.5f) {
        // Strong ACh enhances attention and consolidation
        pImpl->attentionBoost = (pImpl->level - 0.5f) * 2.0f;  // 0 to 1 range
        pImpl->consolidationBoost = pImpl->attentionBoost * 0.8f;
        pImpl->plasticityBoost = pImpl->attentionBoost * 0.6f;
    } else {
        // Low ACh reduces attention effects
        pImpl->attentionBoost = 0.0f;
        pImpl->consolidationBoost = 0.0f;
        pImpl->plasticityBoost = 0.0f;
    }
}

void Acetylcholine::boostAttention(float amount) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
}

void Acetylcholine::consolidateMemory() {
    if (pImpl->brain && pImpl->level > 0.3f) {
        // Enhance memory consolidation in working memory
        auto* workingMemory = pImpl->brain->getWorkingMemory();
        if (workingMemory) {
            // Increase memory trace strength based on ACh level
            float consolidationFactor = pImpl->level * pImpl->level;  // Non-linear effect
            // (Working memory would typically apply this factor)
        }
        
        // Enhance synaptic consolidation in episodic memory
        auto* episodicMemory = pImpl->brain->getEpisodicMemory();
        if (episodicMemory) {
            // Recent memories get strengthened by ACh
            // (Episodic memory would typically apply this effect)
        }
    }
}

void Acetylcholine::enhancePlasticity(float amount) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionBoost = 0.0f;
    pImpl->consolidationBoost = 0.0f;
    pImpl->plasticityBoost = 0.0f;
}

} // namespace nlm