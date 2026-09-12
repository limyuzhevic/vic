#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    // Norepinephrine dynamics state
    float level;                    // Current NE concentration [0, 1]
    float baseline;                 // Baseline NE level
    float peak;                     // Maximum NE level
    float decayRate;                // Rate of decay towards baseline
    float releaseRate;              // Rate of increase on signals
    float adaptationTime;           // Time constant for adaptation
    float arousalInput;             // Arousal/vigilance input
    float noveltyInput;              // Response to novel stimuli
    float history;                  // Rolling average of recent level
    float historyWeight;            // Weight for exponential moving average
    float thresholdLow;             // Low threshold for signaling
    float thresholdHigh;            // High threshold for strong signaling
    bool isAlert;                   // Whether NE is in alert mode
    
    Impl() 
        : level(0.0f)
        , baseline(0.015f)           // Typical baseline NE ~15 nM
        , peak(1.0f)
        , decayRate(0.06f)          // Decay time constant ~16.7 steps
        , releaseRate(1.3f)          // Moderate to fast release
        , adaptationTime(25.0f)      // Moderate adaptation
        , arousalInput(0.0f)
        , noveltyInput(0.0f)
        , history(0.0f)
        , historyWeight(0.12f)
        , thresholdLow(0.1f)
        , thresholdHigh(0.4f)
        , isAlert(false) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine neuromodulator initialized");
}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for attention and learning
    // Higher NE enhances signal-to-noise ratio and focus
    float plasticity = 0.4f + 0.6f * pImpl->level;
    return std::clamp(plasticity, 0.4f, 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtf = static_cast<float>(dt);
    
    // Update rolling average
    pImpl->history = pImpl->history * (1.0f - pImpl->historyWeight) + 
                     pImpl->level * pImpl->historyWeight;
    
    // Decay towards baseline
    float decayAmount = pImpl->decayRate * dtf;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Apply arousal input
    if (pImpl->arousalInput > 0.0f) {
        float arousalUpdate = pImpl->releaseRate * pImpl->arousalInput * dtf;
        pImpl->level += arousalUpdate;
        pImpl->level = std::min(pImpl->peak, pImpl->level);
        
        // Detect alert mode
        if (pImpl->level > pImpl->thresholdHigh) {
            pImpl->isAlert = true;
        } else if (pImpl->level < pImpl->thresholdLow) {
            pImpl->isAlert = false;
        }
    }
    
    // Apply novelty input
    if (pImpl->noveltyInput > 0.0f) {
        // NE responds to novel and surprising stimuli
        float noveltyUpdate = 0.25f * pImpl->noveltyInput * dtf;
        pImpl->level += noveltyUpdate;
        pImpl->level = std::min(pImpl->peak, pImpl->level);
        pImpl->isAlert = true;
    }
    
    // Reset inputs
    pImpl->arousalInput = 0.0f;
    pImpl->noveltyInput = 0.0f;
    
    // Log significant changes
    if (pImpl->isAlert && pImpl->level > 0.5f) {
        NLM_LOG_DEBUG("Norepinephrine alert mode: level = " + std::to_string(pImpl->level));
    }
}

void Norepinephrine::increaseVigilance(float& alertness) const {
    // NE modulates vigilance and attention to important stimuli
    float neEffect = pImpl->level * 0.9f;  // Very strong effect on vigilance
    alertness += neEffect;
}

void Norepinephrine::enhanceResponseSelection(float& responseBias) const {
    // NE biases toward salient responses
    float neEffect = pImpl->level * 0.7f;
    responseBias += neEffect * 0.5f;
}

} // namespace nlm