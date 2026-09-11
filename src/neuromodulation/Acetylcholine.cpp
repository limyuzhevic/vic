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
    float cholinergicGain;
    float attentionWeight;
    float memoryConsolidationBoost;
    std::vector<float> firingHistory;
    Timestamp lastActivation;
    
    Impl() : level(0.0f), baseline(0.001f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             cholinergicGain(1.5f), attentionWeight(1.0f), memoryConsolidationBoost(1.2f),
             lastActivation(0) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine system initialized - attention and memory modulation ready");
}

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
    // Enhanced cholinergic modulation: higher ACh increases plasticity
    // ACh enhances LTP through muscarinic receptor activation
    // Modulates NMDA receptor conductance and increases calcium influx
    float attentionModulation = pImpl->attentionWeight * std::pow(pImpl->level, 0.8f);
    float memoryModulation = pImpl->memoryConsolidationBoost * pImpl->level * 0.5f;
    return 0.5f + attentionModulation * 0.3f + memoryModulation * 0.2f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline, with recovery dynamics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Maintain firing history for plasticity modulation
    if (pImpl->level > 0.1f) {
        pImpl->firingHistory.push_back(pImpl->level);
        if (pImpl->firingHistory.size() > 100) {
            pImpl->firingHistory.erase(pImpl->firingHistory.begin());
        }
    }
}

void Acetylcholine::signalAttention(float stimulusSalience) {
    // Respond to salient stimuli - enhances encoding of novel information
    float burstAmount = stimulusSalience * pImpl->releaseRate * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    pImpl->lastActivation = pImpl->lastActivation + dt; // This is a placeholder, should be timestamp
    NLM_LOG_INFO("Acetylcholine burst in response to salient stimulus");
}

void Acetylcholine::enhanceMemoryConsolidation(float memoryStrength) {
    // Boost memory consolidation through cholinergic modulation
    // ACh promotes LTP and stabilizes memory traces
    float consolidationBoost = memoryStrength * pImpl->memoryConsolidationBoost;
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationBoost * 0.5f);
}

} // namespace nlm