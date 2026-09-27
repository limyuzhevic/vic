#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;                 // Current concentration (0.0 to 1.0)
    float baseline;              // Baseline concentration
    float targetLevel;           // Target concentration (for learning)
    float attentionWeight;       // How much attention affects ACh
    float memoryConsolidationRate; // Rate of memory consolidation
    
    // For attention modulation
    float currentAttention;      // Current attention level
    float attentionHistory[10];   // Recent attention samples
    size_t historyIndex;         // Current position in history array
    
    // For reward prediction error integration
    float rewardPredictionError;  // RPE signal for ACh modulation
    float learningRate;           // Learning rate for attention
    
    // Statistics
    float totalAttentionBoost;    // Total attention boost applied
    uint64_t attentionUpdates;    // Number of attention updates
    
    Impl() : level(0.05f), baseline(0.05f), targetLevel(0.05f),
             attentionWeight(0.8f), memoryConsolidationRate(0.3f),
             currentAttention(0.0f), historyIndex(0), rewardPredictionError(0.0f),
             learningRate(0.01f), totalAttentionBoost(0.0f), attentionUpdates(0) {
        for (int i = 0; i < 10; ++i) {
            attentionHistory[i] = 0.0f;
        }
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine (ACh)";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity based on attention
    // Higher attention = stronger plasticity for attended stimuli
    float attentionModulation = 1.0f + pImpl->attentionWeight * pImpl->currentAttention;
    float rewardModulation = 1.0f + 0.5f * std::max(0.0f, pImpl->rewardPredictionError);
    
    return attentionModulation * rewardModulation;
}

void Acetylcholine::update(TimestepDuration dt) {
    /*
     * Update ACh concentration based on:
     * 1. Attention: ACh increases when attending to novel stimuli
     * 2. Reward prediction error: ACh encodes RPE (like dopamine)
     * 3. Novelty: ACh increases with novel sensory input
     * 4. Memory consolidation demands
     */
    
    // Decay ACh back to baseline
    float decayRate = 0.1f * dt;
    pImpl->level += (pImpl->baseline - pImpl->level) * decayRate;
    
    // Apply reward prediction error signal (positive RPE increases ACh)
    if (std::abs(pImpl->rewardPredictionError) > 0.01f) {
        float rpeModulation = pImpl->rewardPredictionError * 0.5f;
        pImpl->level += rpeModulation * dt;
        pImpl->rewardPredictionError = 0.0f;
    }
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.01f, 0.5f);
    
    // Update statistics
    pImpl->attentionUpdates++;
}

void Acetylcholine::signalAttention(float attention) {
    /*
     * Update attention level and modulate ACh accordingly
     * Attention is a learned process that influences ACh release
     */
    pImpl->currentAttention = attention;
    
    // Update attention history
    pImpl->attentionHistory[pImpl->historyIndex] = attention;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    
    // Calculate average recent attention
    float avgAttention = 0.0f;
    for (int i = 0; i < 10; ++i) {
        avgAttention += pImpl->attentionHistory[i];
    }
    avgAttention /= 10.0f;
    
    // ACh increases attention to novel stimuli
    float noveltyBoost = std::max(0.0f, 1.0f - avgAttention);
    float boostLevel = noveltyBoost * pImpl->attentionWeight * 0.1f;
    
    pImpl->level += boostLevel;
    pImpl->totalAttentionBoost += boostLevel;
}

void Acetylcholine::signalRewardPredictionError(float error) {
    pImpl->rewardPredictionError = error;
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->currentAttention;
}

float Acetylcholine::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate;
}

void Acetylcholine::setMemoryConsolidationRate(float rate) {
    pImpl->memoryConsolidationRate = std::clamp(rate, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionWeight() const {
    return pImpl->attentionWeight;
}

void Acetylcholine::setAttentionWeight(float weight) {
    pImpl->attentionWeight = std::clamp(weight, 0.0f, 2.0f);
}

float Acetylcholine::getTotalAttentionBoost() const {
    return pImpl->totalAttentionBoost;
}

uint64_t Acetylcholine::getAttentionUpdates() const {
    return pImpl->attentionUpdates;
}

} // namespace nlm