#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalLevel;
    float stressResponse;
    float locusCoeruleusGain;
    float decayRate;
    float baseline;
    float peak;
    float fatigueRate;
    bool stressActive;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalLevel(0.0f)
        , stressResponse(0.0f)
        , locusCoeruleusGain(1.0f)
        , decayRate(0.15f)
        , baseline(0.05f)
        , peak(3.0f)
        , fatigueRate(0.01f)
        , stressActive(false) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Calculate arousal based on level
    pImpl->arousalLevel = 0.5f * pImpl->level * (1.0f + pImpl->stressResponse);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine enhances plasticity under stress/arousal
    // Strong arousal increases plasticity for novel/imporant stimuli
    float arousalMod = std::min(2.0f, 1.0f + pImpl->arousalLevel);
    float stressMod = pImpl->stressActive ? 1.5f : 1.0f;
    return 0.2f + 0.8f * arousalMod * stressMod;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decayFactor = std::pow(1.0f - pImpl->decayRate, static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Apply arousal effects on neural excitability
    if (pImpl->brain && pImpl->arousalLevel > 0.0f) {
        // Boost excitatory drive across neural populations
        float arousalBoost = pImpl->arousalLevel * 0.5f;
        
        // Apply to motor neurons for action readiness
        pImpl->brain->injectCurrentToNeurons(NeuronType::Motor, arousalBoost * 2.0f);
        
        // Apply to internal neurons for enhanced processing
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, arousalBoost);
        
        // Apply to sensory neurons for increased sensitivity
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, arousalBoost * 0.5f);
    }
    
    // Handle fatigue accumulation
    if (pImpl->level > 0.5f) {
        // Fatigue accumulates during high norepinephrine levels
        static float fatigue = 0.0f;
        fatigue += pImpl->fatigueRate * static_cast<float>(dt);
        pImpl->level = std::max(0.0f, pImpl->level - fatigue);
    }
}

void Norepinephrine::triggerStressResponse(float magnitude, float duration) {
    // Activate stress response system
    pImpl->stressActive = true;
    pImpl->stressResponse = magnitude;
    pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * 2.0f);
    
    // Prepare for fight/flight response
    if (pImpl->brain) {
        // Rapidly boost motor neuron readiness
        pImpl->brain->injectCurrentToNeurons(NeuronType::Motor, magnitude * 3.0f);
    }
}

void Norepinephrine::resetStress() {
    // Gradually reduce stress response
    pImpl->stressActive = false;
    pImpl->stressResponse = std::max(0.0f, pImpl->stressResponse - 0.2f);
    pImpl->level = std::max(0.0f, pImpl->level - 0.1f);
}

bool Norepinephrine::isStressed() const {
    return pImpl->stressActive && pImpl->level > 0.5f;
}

} // namespace nlm