#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;  // For alertness and vigilance
    float locusCoeruleusInput;  // Simulated LC neuron input
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.5f)
        , arousalLevel(0.0f)
        , locusCoeruleusInput(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update arousal based on norepinephrine level
    pImpl->arousalLevel = level * 1.2f;  // Strong arousal effect
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates attention and working memory
    // Optimal levels (around 0.5) maximize learning, very high levels cause stress
    float optimalLevel = 0.5f;
    float deviation = std::abs(pImpl->level - optimalLevel);
    
    // Inverted U-shaped function: optimal at 0.5, decreasing at extremes
    float plasticity = 1.0f - deviation * 2.0f;  // Range: 1.0 to -1.0
    return std::max(0.1f, plasticity);  // Minimum 0.1 to maintain some plasticity
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // If brain is available, update arousal effects
    if (pImpl->brain) {
        // Apply arousal modulation to neural excitability
        if (pImpl->arousalLevel > 0.3f) {
            // Enhanced alertness increases neural responsiveness
            float alertnessBoost = pImpl->arousalLevel * 0.4f;
            
            // Modulate attention system
            if (pImpl->brain->getAttention()) {
                // Increase attentional focus
                pImpl->brain->getAttention()->setInhibitionStrength(
                    0.5f - pImpl->arousalLevel * 0.3f);  // Reduced inhibition with higher arousal
            }
        }
        
        // Stress response at high levels
        if (pImpl->level > 0.8f) {
            // High NE causes stress - reduce plasticity
            if (pImpl->brain->getDopamine()) {
                // Reduce dopamine temporarily
                float currentDA = pImpl->brain->getDopamine()->getLevel();
                pImpl->brain->getDopamine()->setLevel(std::max(0.0f, currentDA - 0.1f));
            }
        }
    }
}

void Norepinephrine::signalAlertness(float alertnessStrength) {
    // Signal increased alertness or stress
    float newLevel = std::min(pImpl->peak, pImpl->level + alertnessStrength * pImpl->releaseRate);
    setLevel(newLevel);
}

void Norepinephrine::signalFocus(float focusStrength) {
    // Signal focused attention
    float focusBoost = focusStrength * 0.4f;
    float newLevel = std::min(pImpl->peak, pImpl->level + focusBoost);
    setLevel(newLevel);
}

void Norepinephrine::signalStress(float stressLevel) {
    // Signal stress or threat
    if (stressLevel > 0.0f) {
        float stressBoost = stressLevel * pImpl->releaseRate * 1.5f;  // Stress has amplified effect
        float newLevel = std::min(pImpl->peak, pImpl->level + stressBoost);
        setLevel(newLevel);
        
        // High stress inhibits memory formation
        if (pImpl->brain && pImpl->brain->getWorkingMemory()) {
            pImpl->brain->getWorkingMemory()->strengthenMemory(0.5f);  // Weaken memory traces
        }
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized - arousal and vigilance");
}

} // namespace nlm
