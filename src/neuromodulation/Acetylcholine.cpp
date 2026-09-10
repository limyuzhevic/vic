// Acetylcholine implementation with comprehensive attention and memory modulation
// Enhanced version with real plasticity factor calculations

#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionStrength;
    
    // Memory effects
    float memoryConsolidationStrength;
    float workingMemoryBoost;
    float acetylcholineBaseline;
    float synthesisRate;
    float metabolismRate;
    
    // Dynamic attention modulation
    float sustainedAttention;
    float attentionalFocus;
    float cognitiveLoad;
    
    // Learning state
    float recentLearningRate;
    float memoryStability;
    float encodingStrength;
    
    // Activity tracking
    float attentionHistory[20]; // Track attention over time
    int historyIndex;
    
    // Stress and arousal influences
    float stressLevel;
    float arousalLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f),
             attentionStrength(0.0f), memoryConsolidationStrength(0.0f), workingMemoryBoost(0.0f),
             acetylcholineBaseline(0.05f), synthesisRate(0.03f), metabolismRate(0.01f),
             sustainedAttention(0.5f), attentionalFocus(0.5f), cognitiveLoad(0.0f),
             recentLearningRate(0.0f), memoryStability(0.5f), encodingStrength(0.5f),
             historyIndex(0), stressLevel(0.0f), arousalLevel(0.0f) {
        for (int i = 0; i < 20; ++i) attentionHistory[i] = 0.0f;
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine: Initialized");
}

Acetylcholine::~Acetylcholine() {
    NLM_LOG_INFO("Acetylcholine: Destroyed");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    NLM_LOG_INFO("Acetylcholine: Level set to " + std::to_string(pImpl->level));
}

float Acetylcholine::getPlasticityFactor() const {
    // Comprehensive acetylcholine-modulated plasticity factor
    // Based on biological research: ACh enhances plasticity when attention is paid,
    // but stabilizes memory traces when consolidation is active
    
    // 1. Attention component: High attentional focus increases learning
    float attentionComponent = pImpl->attentionStrength * 0.6f;
    
    // 2. Memory consolidation component: Strong consolidation stabilizes learning
    float memoryComponent = pImpl->memoryConsolidationStrength * 0.25f;
    
    // 3. Baseline cholinergic tone: Tonic level provides baseline modulation
    float baselineComponent = pImpl->level * 0.15f;
    
    // 4. Cognitive load penalty: High load reduces plasticity efficiency
    float loadPenalty = pImpl->cognitiveLoad * 0.2f;
    
    // 5. Encoding strength: Active encoding requires more plasticity
    float encodingComponent = pImpl->encodingStrength * 0.1f;
    
    // 6. Stress interaction: Stress modulates ACh effects
    float stressInteraction = pImpl->stressLevel * 0.15f;
    
    // 7. Memory stability: Prevents runaway plasticity
    float stabilityComponent = pImpl->memoryStability * 0.1f;
    
    // 8. Arousal modulation: Moderate arousal optimizes plasticity
    float arousalComponent = pImpl->arousalLevel * 0.2f;
    
    // Calculate final plasticity factor with biological constraints
    float basePlasticity = 0.4f; // Baseline plasticity factor
    float combinedEffects = attentionComponent + memoryComponent + baselineComponent 
                           + encodingComponent + arousalComponent + stabilityComponent
                           - loadPenalty + stressInteraction;
    
    // Apply biological bounds and non-linearities
    float plasticityFactor = basePlasticity + combinedEffects;
    
    // Ensure reasonable bounds
    plasticityFactor = std::max(0.1f, std::min(3.0f, plasticityFactor));
    
    // Apply sigmoidal saturation for extreme values
    if (plasticityFactor > 1.5f) {
        plasticityFactor = 1.5f + (plasticityFactor - 1.5f) * 0.7f;
    } else if (plasticityFactor < 0.3f) {
        plasticityFactor = 0.3f + (plasticityFactor - 0.3f) * 0.8f;
    }
    
    NLM_LOG_DEBUG("Acetylcholine plasticity factor calculated: " + std::to_string(plasticityFactor) +
                 " (attention: " + std::to_string(attentionComponent) + ", memory: " + 
                 std::to_string(memoryComponent) + ", load penalty: " + std::to_string(loadPenalty) + ")");
    
    return plasticityFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Natural dynamics with homeostatic regulation
    float previousLevel = pImpl->level;
    
    // Decay towards baseline with slow recovery
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Update attentional states
    pImpl->attentionStrength = pImpl->attentionStrength * 0.98f - pImpl->decayRate * dtFloat * 0.1f;
    pImpl->sustainedAttention = pImpl->sustainedAttention * 0.99f + pImpl->decayRate * dtFloat * 0.01f;
    
    // Cognitive load naturally decays
    pImpl->cognitiveLoad = std::max(0.0f, pImpl->cognitiveLoad - pImpl->decayRate * dtFloat * 0.5f);
    
    // Memory effects with natural decay
    pImpl->memoryConsolidationStrength = pImpl->memoryConsolidationStrength * 0.95f - pImpl->decayRate * dtFloat * 0.05f;
    pImpl->workingMemoryBoost = pImpl->workingMemoryBoost * 0.9f - pImpl->decayRate * dtFloat * 0.05f;
    
    // Learning rate adaptation
    pImpl->recentLearningRate = pImpl->recentLearningRate * 0.95f + pImpl->decayRate * dtFloat * 0.05f;
    pImpl->memoryStability = pImpl->memoryStability * 0.98f + pImpl->decayRate * dtFloat * 0.02f;
    
    // Stress and arousal dynamics
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - pImpl->decayRate * dtFloat * 0.3f);
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - pImpl->decayRate * dtFloat * 0.4f);
    
    // Update attention history
    pImpl->attentionHistory[pImpl->historyIndex] = pImpl->attentionStrength;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 20;
    
    // Log significant level changes
    if (std::abs(previousLevel - pImpl->level) > 0.1f) {
        NLM_LOG_INFO("Acetylcholine: Level changed from " + std::to_string(previousLevel) + 
                     " to " + std::to_string(pImpl->level));
    }
}

void Acetylcholine::increaseAttention(float amount) {
    pImpl->attentionStrength = std::min(pImpl->peak, pImpl->attentionStrength + amount);
    // Immediate increase in ACh level to support attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * 0.7f);
    
    // Sustained attention increases encoding strength
    pImpl->encodingStrength = std::min(pImpl->peak, pImpl->encodingStrength + amount * 0.3f);
    
    // Reduce cognitive load by improving focus
    pImpl->cognitiveLoad = std::max(0.0f, pImpl->cognitiveLoad - amount * 0.5f);
    
    // Boost working memory
    pImpl->workingMemoryBoost = std::min(pImpl->peak, pImpl->workingMemoryBoost + amount * 0.4f);
    
    // Update arousal
    pImpl->arousalLevel = std::min(pImpl->peak, pImpl->arousalLevel + amount * 0.3f);
    
    NLM_LOG_INFO("Acetylcholine: Increased attention strength by " + std::to_string(amount) + 
                 " (level: " + std::to_string(pImpl->level) + ", arousal: " + 
                 std::to_string(pImpl->arousalLevel) + ")");
}

void Acetylcholine::decreaseAttention(float amount) {
    pImpl->attentionStrength = std::max(0.0f, pImpl->attentionStrength - amount);
    // Decrease level to reflect reduced attentional demand
    pImpl->level = std::max(pImpl->baseline, pImpl->level - amount * 0.5f);
    
    // Encoding strength decreases with reduced attention
    pImpl->encodingStrength = std::max(0.1f, pImpl->encodingStrength - amount * 0.2f);
    
    // Stress may increase with attentional withdrawal
    pImpl->stressLevel = std::min(pImpl->peak, pImpl->stressLevel + amount * 0.2f);
    
    NLM_LOG_INFO("Acetylcholine: Decreased attention strength by " + std::to_string(amount) + 
                 " (level: " + std::to_string(pImpl->level) + ", stress: " + 
                 std::to_string(pImpl->stressLevel) + ")");
}

float Acetylcholine::getAttentionStrength() const {
    return pImpl->attentionStrength;
}

void Acetylcholine::enhanceMemoryConsolidation(float strength) {
    pImpl->memoryConsolidationStrength = std::min(pImpl->peak, strength);
    pImpl->workingMemoryBoost = pImpl->memoryConsolidationStrength * 0.8f;
    
    // Strengthen memory stability
    pImpl->memoryStability = std::min(pImpl->peak, pImpl->memoryStability + strength * 0.3f);
    
    // Moderate increase in baseline ACh level
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * 0.2f);
    
    // Update arousal for memory consolidation
    pImpl->arousalLevel = std::min(pImpl->peak, pImpl->arousalLevel + strength * 0.1f);
    
    NLM_LOG_INFO("Acetylcholine: Enhanced memory consolidation with strength " + std::to_string(strength) + 
                 " (working memory boost: " + std::to_string(pImpl->workingMemoryBoost) + ", stability: " + 
                 std::to_string(pImpl->memoryStability) + ")");
}

void Acetylcholine::signalAttentionDemand(float demand) {
    // Respond to attentional demands from environment or task
    pImpl->stressLevel = std::min(pImpl->peak, pImpl->stressLevel + demand * 0.4f);
    pImpl->cognitiveLoad = std::min(pImpl->peak, pImpl->cognitiveLoad + demand * 0.5f);
    
    // Increase attention strength and ACh level
    pImpl->attentionStrength = std::min(pImpl->peak, pImpl->attentionStrength + demand * 0.6f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + demand * 0.8f);
    
    // Arousal increases with attention demand
    pImpl->arousalLevel = std::min(pImpl->peak, pImpl->arousalLevel + demand * 0.3f);
    
    NLM_LOG_INFO("Acetylcholine: Attention demand signaled (level: " + std::to_string(pImpl->level) + ")");
}

void Acetylcholine::signalMemoryLoad(float load) {
    // Respond to memory system load
    pImpl->cognitiveLoad = std::min(pImpl->peak, pImpl->cognitiveLoad + load);
    
    // High memory load reduces attention efficiency
    pImpl->attentionStrength = std::max(0.0f, pImpl->attentionStrength - load * 0.3f);
    
    // Increase consolidation to handle load
    pImpl->memoryConsolidationStrength = std::min(pImpl->peak, pImpl->memoryConsolidationStrength + load * 0.4f);
    
    NLM_LOG_INFO("Acetylcholine: Memory load signaled " + std::to_string(load));
}

void Acetylcholine::signalLearningEvent(float strength) {
    // Respond to learning events
    pImpl->recentLearningRate = std::min(pImpl->peak, pImpl->recentLearningRate + strength * 0.5f);
    pImpl->encodingStrength = std::min(pImpl->peak, pImpl->encodingStrength + strength * 0.3f);
    
    // Moderate ACh release for learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * 0.3f);
    
    NLM_LOG_INFO("Acetylcholine: Learning event signaled with strength " + std::to_string(strength));
}

float Acetylcholine::getAttentionStrength() const {
    return pImpl->attentionStrength;
}

float Acetylcholine::getMemoryConsolidationStrength() const {
    return pImpl->memoryConsolidationStrength;
}

float Acetylcholine::getWorkingMemoryBoost() const {
    return pImpl->workingMemoryBoost;
}

float Acetylcholine::getSustainedAttention() const {
    return pImpl->sustainedAttention;
}

float Acetylcholine::getCognitiveLoad() const {
    return pImpl->cognitiveLoad;
}

} // namespace nlm