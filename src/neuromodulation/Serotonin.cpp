#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    // Serotonin dynamics state
    float level;                    // Current 5-HT concentration [0, 1]
    float baseline;                 // Baseline 5-HT level
    float peak;                     // Maximum 5-HT level
    float decayRate;                // Rate of decay towards baseline
    float releaseRate;              // Rate of increase on signals
    float adaptationTime;           // Time constant for adaptation
    float moodInput;                // Mood regulation input
    float inhibitionInput;          // Behavioral inhibition input
    float history;                  // Rolling average of recent level
    float historyWeight;            // Weight for exponential moving average
    float thresholdLow;             // Low threshold for signaling
    float thresholdHigh;            // High threshold for strong signaling
    bool isCalm;                    // Whether 5-HT is in calm/behaved state
    
    Impl() 
        : level(0.0f)
        , baseline(0.025f)           // Typical baseline 5-HT ~25 nM
        , peak(1.0f)
        , decayRate(0.07f)          // Decay time constant ~14.3 steps
        , releaseRate(1.0f)          // Moderate release
        , adaptationTime(40.0f)      // Slow adaptation
        , moodInput(0.0f)
        , inhibitionInput(0.0f)
        , history(0.0f)
        , historyWeight(0.08f)
        , thresholdLow(0.05f)
        , thresholdHigh(0.3f)
        , isCalm(false) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin neuromodulator initialized");
}

Serotonin::~Serotonin() {
    delete pImpl;
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for behavioral inhibition
    // Higher 5-HT reduces impulsivity and enhances behavioral flexibility
    float plasticity = 0.5f + 0.5f * pImpl->level;
    return std::clamp(plasticity, 0.5f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    float dtf = static_cast<float>(dt);
    
    // Update rolling average
    pImpl->history = pImpl->history * (1.0f - pImpl->historyWeight) + 
                     pImpl->level * pImpl->historyWeight;
    
    // Decay towards baseline
    float decayAmount = pImpl->decayRate * dtf;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Apply mood input
    if (pImpl->moodInput > 0.0f) {
        float moodUpdate = pImpl->releaseRate * pImpl->moodInput * dtf;
        pImpl->level += moodUpdate;
        pImpl->level = std::min(pImpl->peak, pImpl->level);
        
        // Detect calm state
        if (pImpl->level > pImpl->thresholdHigh) {
            pImpl->isCalm = true;
        } else if (pImpl->level < pImpl->thresholdLow) {
            pImpl->isCalm = false;
        }
    }
    
    // Apply inhibition input
    if (pImpl->inhibitionInput > 0.0f) {
        // 5-HT inhibits impulsive responses
        float inhibitionUpdate = 0.15f * pImpl->inhibitionInput * dtf;
        pImpl->level += inhibitionUpdate;
        pImpl->level = std::min(pImpl->peak, pImpl->level);
        pImpl->isCalm = true;
    }
    
    // Reset inputs
    pImpl->moodInput = 0.0f;
    pImpl->inhibitionInput = 0.0f;
    
    // Log significant changes
    if (pImpl->isCalm && pImpl->level > 0.25f) {
        NLM_LOG_DEBUG("Serotonin calm state: level = " + std::to_string(pImpl->level));
    }
}

void Serotonin::regulateMood(float& rewardValuation) const {
    // 5-HT modulates mood and reward valuation
    float htEffect = pImpl->level * 0.6f;
    rewardValuation += htEffect * 0.3f;
}

void Serotonin::controlImpulsivity(float& decisionThreshold) const {
    // 5-HT controls impulsivity by raising decision thresholds
    float htEffect = pImpl->level * 0.8f;
    decisionThreshold += htEffect * 0.5f;
}

} // namespace nlm