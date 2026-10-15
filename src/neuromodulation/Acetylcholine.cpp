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
    float reuptakeRate;
    float receptorDensity;
    float receptorSaturation;
    float cholinePool;
    float acetylCoA;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.8f)
        , reuptakeRate(0.2f)
        , receptorDensity(1.0f)
        , receptorSaturation(0.8f)
        , cholinePool(1.0f)
        , acetylCoA(0.5f) {}
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
    // Acetylcholine modulates attention-based learning and memory consolidation
    float effectiveLevel = std::min(pImpl->level / pImpl->receptorSaturation, 1.0f);
    
    // ACh enhances signal-to-noise ratio through nicotinic receptors
    // Improves encoding of attended stimuli
    float attentionGain = 1.0f + 1.5f * effectiveLevel;
    
    // ACh modulates working memory capacity
    float memoryCapacity = 1.0f + 0.8f * effectiveLevel;
    
    // ACh promotes memory consolidation during rest periods
    float consolidationFactor = 0.5f + 0.5f * effectiveLevel;
    
    // Combined effect on plasticity
    float plasticityFactor = attentionGain * memoryCapacity * consolidationFactor;
    
    return std::max(0.5f, std::min(plasticityFactor, 3.0f));
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Multiple clearance mechanisms
    float decay = pImpl->decayRate * dtFloat;
    
    // Acetylcholinesterase breakdown
    float breakdown = decay * 0.8f;
    
    // Choline reuptake
    float reuptake = pImpl->reuptakeRate * pImpl->level * dtFloat;
    
    // Replenishment from choline pool
    float replenishment = 0.01f * dtFloat;
    
    // Apply clearance
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - breakdown - reuptake);
    
    // Update precursor availability
    pImpl->cholinePool = std::min(1.0f, pImpl->cholinePool + replenishment);
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Attention signaling through cholinergic pathways
    // Phasic bursts for novel or behaviorally relevant stimuli
    
    // Probability of cholinergic burst based on attentional demand
    float burstProbability = std::min(1.0f, attentionStrength * pImpl->releaseRate);
    
    // Burst magnitude scaled by attentional strength
    float burstSize = attentionStrength * pImpl->releaseRate * (1.0f + pImpl->cholinePool * 0.3f);
    
    // Apply burst with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize);
}

void Acetylcholine::signalMemoryConsolidation(float memoryStrength) {
    // Memory consolidation signaling through cholinergic mechanisms
    // Promotes synaptic strengthening during rest and sleep
    
    // Consolidation boost scaled by memory strength and precursor availability
    float consolidationBoost = memoryStrength * pImpl->releaseRate * 
                               (1.0f + pImpl->acetylCoA * 0.5f);
    
    // Apply with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationBoost);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->cholinePool = 1.0f;
    pImpl->acetylCoA = 0.5f;
}

} // namespace nlm
