#include "../neuromodulation/Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;                    // Current norepinephrine concentration
    float arousalLevel;            // Overall arousal/vigilance
    float locusCoeruleusInput;      // Input from LC neurons
    float stressResponse;           // Stress/excitement response
    float encodingBoost;            // Effect on sensory encoding
    SimulationStep lastActivation;  // Last activation time
    
    Impl()
        : level(0.0f)
        , arousalLevel(1.0f)
        , locusCoeruleusInput(0.0f)
        , stressResponse(0.0f)
        , encodingBoost(1.0f)
        , lastActivation(0)
    {}
};

Norepinephrine::Norepinephrine()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("Norepinephrine system initialized");
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(level, 1.0f));
    
    // Higher NE increases arousal and encoding boost
    pImpl->arousalLevel = 1.0f + pImpl->level * 0.8f;
    pImpl->encodingBoost = 1.0f + pImpl->level * 0.6f;
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity based on arousal state
    return pImpl->encodingBoost * pImpl->arousalLevel;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay norepinephrine level
    pImpl->level *= (1.0f - dt * 0.04f);
    
    // Arousal decays more slowly
    pImpl->arousalLevel = 1.0f + (pImpl->arousalLevel - 1.0f) * (1.0f - dt * 0.05f);
}

void Norepinephrine::signalAlert(float stimulusIntensity) {
    // Signal alertness-inducing stimulus
    if (stimulusIntensity > 0.01f) {
        float burst = std::min(1.0f, stimulusIntensity * 0.5f);
        setLevel(getLevel() + burst);
        
        // Record activation time
        pImpl->lastActivation = 0; // Would use actual simulation time
        
        NLM_LOG_INFO("Norepinephrine burst: intensity=" + std::to_string(stimulusIntensity) + 
                     " level=" + std::to_string(getLevel()) + " arousal=" + std::to_string(pImpl->arousalLevel));
    }
}

void Norepinephrine::signalStress(float stressLevel) {
    // Signal stress response
    if (stressLevel > 0.0f) {
        setLevel(getLevel() + stressLevel * 0.3f);
        pImpl->stressResponse = stressLevel;
        
        NLM_LOG_INFO("Stress response: stress=" + std::to_string(stressLevel) + 
                     " level=" + std::to_string(getLevel()));
    }
}

float Norepinephrine::getArousal() const {
    return pImpl->arousalLevel;
}

bool Norepinephrine::isHighARousal() const {
    return getArousal() > 2.0f;  // More than 2x baseline
}

} // namespace nlm
