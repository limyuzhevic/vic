#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float tonicLevel;
    float phasicLevel;
    float attentionGain;
    float memoryConsolidation;
    
    // Temporal dynamics
    float attentionSignal;
    float memoryTrace;
    float decayRate;
    float integrationRate;
    float consolidationDelay;
    
    // Time tracking
    float timeSinceLastRelease;
    float lastAttentionUpdate;
    
    // Memory systems integration
    std::vector<float> recentMemoryTrace;
    float memoryCapacity;
    float consolidationThreshold;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), baseline(0.1f), tonicLevel(0.1f), phasicLevel(0.0f),
             attentionGain(1.0f), memoryConsolidation(0.5f), attentionSignal(0.0f),
             memoryTrace(0.0f), decayRate(0.05f), integrationRate(0.2f),
             consolidationDelay(0.5f), timeSinceLastRelease(0.0f),
             lastAttentionUpdate(0.0f), memoryCapacity(100),
             consolidationThreshold(0.7f)
    {
        std::random_device rd;
        rng.seed(rd());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity and memory consolidation
    // Phasic ACh boosts plasticity, tonic ACh maintains baseline
    float phasicContribution = pImpl->phasicLevel * 2.0f;
    float tonicContribution = pImpl->tonicLevel;
    
    // Total ACh level
    float totalACh = std::min(pImpl->phasicLevel + tonicContribution, 1.0f);
    
    // Plasticity enhancement (non-linear relationship)
    float plasticity = 0.5f + totalACh * 1.5f;
    
    // Memory consolidation factor
    float memoryFactor = pImpl->memoryConsolidation;
    
    return std::clamp(plasticity * memoryFactor, 0.1f, 3.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Update time tracking
    pImpl->timeSinceLastRelease += timeStep;
    pImpl->lastAttentionUpdate += timeStep;
    
    // Natural decay
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - pImpl->decayRate * timeStep);
    
    // Update phasic component
    pImpl->phasicLevel *= (1.0f - pImpl->decayRate * timeStep);
    
    // Update attention signal
    pImpl->attentionSignal *= (1.0f - pImpl->decayRate * timeStep);
    
    // Update memory trace
    pImpl->memoryTrace *= (1.0f - pImpl->decayRate * timeStep);
    
    // Maintain memory capacity
    if (pImpl->recentMemoryTrace.size() > pImpl->memoryCapacity) {
        pImpl->recentMemoryTrace.erase(pImpl->recentMemoryTrace.begin());
    }
}

void Acetylcholine::consolidateMemory(const std::vector<float>& memoryTrace) {
    if (memoryTrace.empty()) return;
    
    // Add new memory to trace
    pImpl->recentMemoryTrace.insert(pImpl->recentMemoryTrace.end(),
                                    memoryTrace.begin(), memoryTrace.end());
    
    // Maintain capacity
    if (pImpl->recentMemoryTrace.size() > pImpl->memoryCapacity) {
        pImpl->recentMemoryTrace.erase(pImpl->recentMemoryTrace.begin(),
                                       pImpl->recentMemoryTrace.begin() +
                                       (pImpl->recentMemoryTrace.size() - pImpl->memoryCapacity));
    }
    
    // Compute average memory strength
    float totalStrength = 0.0f;
    for (float strength : pImpl->recentMemoryTrace) {
        totalStrength += strength;
    }
    
    float avgStrength = totalStrength / std::max<float>(pImpl->recentMemoryTrace.size(), 1.0f);
    
    // Update consolidation level
    pImpl->memoryConsolidation = std::min(1.0f, avgStrength * 1.2f);
    
    // Generate phasic response for strong memories
    if (avgStrength > pImpl->consolidationThreshold) {
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + 0.5f, 1.0f);
        pImpl->level = std::min(1.0f, pImpl->level + 0.3f);
    }
    
    NLM_LOG_DEBUG("ACh memory consolidation: avg_strength=%.3f, consolidation=%.3f",
                  avgStrength, pImpl->memoryConsolidation);
}

void Acetylcholine::updateAttention(float sensoryInput) {
    // Update attention based on sensory input strength
    pImpl->attentionSignal += std::abs(sensoryInput) * pImpl->attentionGain;
    
    // Limit attention signal
    pImpl->attentionSignal = std::min(pImpl->attentionSignal, 1.0f);
    
    // Time since last update tracking
    float timeSinceUpdate = pImpl->lastAttentionUpdate;
    if (timeSinceUpdate > 0.0f) {
        float attentionRate = pImpl->attentionSignal / timeSinceUpdate;
        pImpl->attentionGain = std::clamp(attentionGain + attentionRate * 0.1f, 0.5f, 2.0f);
    }
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionSignal;
}

float Acetylcholine::getMemoryConsolidationLevel() const {
    return pImpl->memoryConsolidation;
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.5f, 2.0f);
}

void Acetylcholine::setMemoryDecayRate(float decayRate) {
    pImpl->decayRate = std::clamp(decayRate, 0.01f, 0.2f);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->phasicLevel = 0.0f;
    pImpl->attentionSignal = 0.0f;
    pImpl->memoryTrace = 0.0f;
    pImpl->memoryConsolidation = 0.5f;
    pImpl->recentMemoryTrace.clear();
    
    NLM_LOG_INFO("Acetylcholine system reset");
}

} // namespace nlm