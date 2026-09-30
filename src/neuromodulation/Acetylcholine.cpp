#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float reuptakeRate;
    float synthesisRate;
    float attention;
    float workingMemory;
    float memoryConsolidation;
    SimulationStep lastReleased;
    bool acetylcholineBurst;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.5f),
             reuptakeRate(0.4f), synthesisRate(0.04f), attention(0.5f), 
             workingMemory(0.5f), memoryConsolidation(0.5f), lastReleased(0),
             acetylcholineBurst(false) {}
};

Acetylcholine::Acetylcholine()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("Acetylcholine neuromodulator initialized");
}

Acetylcholine::~Acetylcholine() {
    NLM_LOG_INFO("Acetylcholine neuromodulator destroyed");
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
    // Enhanced acetylcholine: modulates attention and memory consolidation
    float basePlasticity = 0.7f;
    float attentionFactor = 0.6f + 0.5f * pImpl->attention;
    float workingMemoryFactor = 0.8f + 0.4f * pImpl->workingMemory;
    float consolidationFactor = 0.9f + 0.6f * pImpl->memoryConsolidation;
    float burstFactor = pImpl->acetylcholineBurst ? 1.5f : 1.0f;
    
    return basePlasticity * attentionFactor * workingMemoryFactor * consolidationFactor * burstFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Dynamic acetylcholine dynamics with burst modes
    float timeFactor = static_cast<float>(dt) / 100.0f;
    
    // Normal decay and reuptake
    pImpl->level -= pImpl->decayRate * timeFactor;
    pImpl->level -= pImpl->reuptakeRate * timeFactor * pImpl->level;
    
    // Continuous synthesis
    pImpl->level += pImpl->synthesisRate * timeFactor;
    
    // Reset burst flag
    pImpl->acetylcholineBurst = false;
    
    // Ensure level stays in valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Update related state variables
    pImpl->attention = std::min(1.0f, pImpl->attention + 0.01f * timeFactor);
    pImpl->workingMemory = std::min(1.0f, pImpl->workingMemory + 0.005f * timeFactor);
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 0.008f * timeFactor);
}

void Acetylcholine::signalAttentionChange(float attentionDelta) {
    pImpl->attention += attentionDelta;
    pImpl->attention = std::clamp(pImpl->attention, 0.0f, 1.0f);
    
    // High attention triggers burst mode
    if (pImpl->attention > 0.9f) {
        pImpl->acetylcholineBurst = true;
        setLevel(getLevel() + 0.3f);  // Burst release
    }
}

void Acetylcholine::signalWorkingMemoryChange(float wmDelta) {
    pImpl->workingMemory += wmDelta;
    pImpl->workingMemory = std::clamp(pImpl->workingMemory, 0.0f, 1.0f);
    
    // Strong working memory enhances plasticity
    if (pImpl->workingMemory > 0.8f) {
        setLevel(getLevel() + 0.2f);
    }
}

void Acetylcholine::signalMemoryConsolidationChange(float consolidationDelta) {
    pImpl->memoryConsolidation += consolidationDelta;
    pImpl->memoryConsolidation = std::clamp(pImpl->memoryConsolidation, 0.0f, 1.0f);
    
    // Strong consolidation improves long-term memory
    if (pImpl->memoryConsolidation > 0.9f) {
        setLevel(getLevel() + 0.25f);
    }
}

float Acetylcholine::getAttention() const {
    return pImpl->attention;
}

float Acetylcholine::getWorkingMemory() const {
    return pImpl->workingMemory;
}

float Acetylcholine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

void Acetylcholine::enhanceAttention(float attentionBoost) {
    // Acetylcholine specifically enhances attention and sensory processing
    pImpl->attention = std::min(1.0f, pImpl->attention + 0.4f * attentionBoost);
    pImpl->workingMemory = std::min(1.0f, pImpl->workingMemory + 0.2f * attentionBoost);
    
    // Trigger burst mode for high attention
    if (pImpl->attention > 0.8f) {
        pImpl->acetylcholineBurst = true;
    }
    
    // Boost level for enhanced plasticity
    setLevel(getLevel() + 0.15f * attentionBoost);
}

void Acetylcholine::enhanceWorkingMemory(float wmBoost) {
    // Acetylcholine specifically enhances working memory capacity
    pImpl->workingMemory = std::min(1.0f, pImpl->workingMemory + 0.35f * wmBoost);
    pImpl->attention = std::min(1.0f, pImpl->attention + 0.15f * wmBoost);
    
    // Improved working memory through higher plasticity
    setLevel(getLevel() + 0.2f * wmBoost);
}

void Acetylcholine::enhanceMemoryConsolidation(float consolidationBoost) {
    // Acetylcholine specifically enhances memory consolidation and stabilization
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 0.4f * consolidationBoost);
    pImpl->workingMemory = std::min(1.0f, pImpl->workingMemory + 0.2f * consolidationBoost);
    
    // Strong consolidation through high plasticity
    if (consolidationBoost > 0.6f) {
        pImpl->acetylcholineBurst = true;
        setLevel(getLevel() + 0.3f);
    }
}

void Acetylcholine::initiateLearningBurst(float intensity) {
    // Acetylcholine triggers learning burst mode
    pImpl->acetylcholineBurst = true;
    setLevel(getLevel() + 0.5f * intensity);  // Large burst
    
    // Simultaneous enhancement of all related functions
    pImpl->attention = std::min(1.0f, pImpl->attention + 0.3f * intensity);
    pImpl->workingMemory = std::min(1.0f, pImpl->workingMemory + 0.25f * intensity);
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 0.2f * intensity);
}

} // namespace nlm