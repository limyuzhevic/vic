#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    // Acetylcholine dynamics state
    float level;                    // Current ACh concentration [0, 1]
    float baseline;                 // Baseline ACh level
    float peak;                     // Maximum ACh level
    float decayRate;                // Rate of decay towards baseline
    float releaseRate;              // Rate of increase on signals
    float adaptationTime;           // Time constant for adaptation
    float attentionInput;           // Attention/bias input
    float workingMemoryInput;       // Working memory update input
    float history;                  // Rolling average of recent level
    float historyWeight;            // Weight for exponential moving average
    float thresholdLow;             // Low threshold for signaling
    float thresholdHigh;            // High threshold for strong signaling
    bool isFocusing;                // Whether ACh is in focused attention mode
    
    Impl() 
        : level(0.0f)
        , baseline(0.01f)           // Typical baseline ACh ~10 nM
        , peak(1.0f)
        , decayRate(0.08f)          // Decay time constant ~12.5 steps
        , releaseRate(1.2f)          // Moderate release
        , adaptationTime(30.0f)      // Fast adaptation
        , attentionInput(0.0f)
        , workingMemoryInput(0.0f)
        , history(0.0f)
        , historyWeight(0.15f)
        , thresholdLow(0.15f)
        , thresholdHigh(0.5f)
        , isFocusing(false) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine neuromodulator initialized");
}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
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
    // ACh modulates plasticity for memory consolidation
    // Higher ACh enhances memory encoding and consolidation
    float plasticity = 0.3f + 0.7f * pImpl->level;
    return std::clamp(plasticity, 0.3f, 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtf = static_cast<float>(dt);
    
    // Update rolling average
    pImpl->history = pImpl->history * (1.0f - pImpl->historyWeight) + 
                     pImpl->level * pImpl->historyWeight;
    
    // Decay towards baseline
    float decayAmount = pImpl->decayRate * dtf;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Apply attention input
    if (pImpl->attentionInput > 0.0f) {
        float attentionUpdate = pImpl->releaseRate * pImpl->attentionInput * dtf;
        pImpl->level += attentionUpdate;
        pImpl->level = std::min(pImpl->peak, pImpl->level);
        
        // Detect focus mode
        if (pImpl->level > pImpl->thresholdHigh) {
            pImpl->isFocusing = true;
        } else if (pImpl->level < pImpl->thresholdLow) {
            pImpl->isFocusing = false;
        }
    }
    
    // Apply working memory input
    if (pImpl->workingMemoryInput > 0.0f) {
        // ACh enhances memory encoding
        float memoryUpdate = 0.2f * pImpl->workingMemoryInput * dtf;
        pImpl->level += memoryUpdate;
        pImpl->level = std::min(pImpl->peak, pImpl->level);
        pImpl->isFocusing = true;
    }
    
    // Reset inputs
    pImpl->attentionInput = 0.0f;
    pImpl->workingMemoryInput = 0.0f;
    
    // Log significant changes
    if (pImpl->isFocusing && pImpl->level > 0.6f) {
        NLM_LOG_DEBUG("Acetylcholine focused attention: level = " + std::to_string(pImpl->level));
    }
}

void Acetylcholine::enhanceSensoryProcessing(float& signalStrength) const {
    // ACh modulates sensory processing for attention
    float achEffect = pImpl->level * 0.8f;  // Strong effect on sensory processing
    signalStrength *= (1.0f + achEffect);
}

void Acetylcholine::modulateWorkingMemory(float& memoryTrace) const {
    // ACh enhances working memory maintenance
    float achEffect = pImpl->level * 0.6f;
    memoryTrace *= (1.0f + achEffect * 0.5f);
}

} // namespace nlm