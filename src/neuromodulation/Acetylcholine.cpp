#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionBoost;
    float memoryConsolidationRate;
    float decayRate;
    float releaseRate;
    
    // Track time since last reward
    float timeSinceLastReward;
    float rewardSensitivity;
    
    Impl() : level(0.0f), baseline(0.1f), attentionBoost(0.3f), 
             memoryConsolidationRate(0.5f), decayRate(0.1f), 
             releaseRate(0.8f), timeSinceLastReward(0.0f), 
             rewardSensitivity(1.0f) {}
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
    // ACh enhances plasticity for attention-related learning
    // and memory consolidation
    float modulation = 0.5f + 0.5f * pImpl->level;
    
    // Boost plasticity more when attention is high
    modulation += pImpl->attentionBoost * pImpl->level;
    
    // Reduce plasticity for unrelated learning (focused attention)
    if (pImpl->level > 0.7f) {
        modulation *= 0.8f;
    }
    
    return std::clamp(modulation, 0.0f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: decays towards baseline but increases with attention/reward
    // Time-dependent learning modulation
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reduce time since last reward
    pImpl->timeSinceLastReward += static_cast<float>(dt);
    
    // ACh enhances memory consolidation over time
    float consolidationEffect = pImpl->memoryConsolidationRate * static_cast<float>(dt);
    // This would affect memory systems in the brain
}

void Acetylcholine::signalAttention(float attentionLevel) {
    // Boost ACh level based on attention demands
    float boost = attentionLevel * pImpl->attentionBoost;
    pImpl->level = std::min(1.0f, pImpl->level + boost);
    
    // High attention improves reward sensitivity
    pImpl->rewardSensitivity = 1.0f + attentionLevel * 0.5f;
}

void Acetylcholine::signalReward(float reward) {
    // ACh responds to reward and improves memory formation
    float rewardBoost = reward * pImpl->releaseRate * pImpl->rewardSensitivity;
    pImpl->level = std::min(1.0f, pImpl->level + rewardBoost);
    
    // Reset time since last reward
    pImpl->timeSinceLastReward = 0.0f;
}

float Acetylcholine::getAttentionBoost() const {
    return pImpl->attentionBoost;
}

float Acetylcholine::getTimeSinceLastReward() const {
    return pImpl->timeSinceLastReward;
}

} // namespace nlm
