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
    float arousalLevel;
    float vigilance;
    float locusCoeruleusGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.05f)  // Baseline arousal
        , baseline(0.05f)
        , peak(0.4f)
        , decayRate(0.03f)
        , releaseRate(0.8f)
        , arousalLevel(1.0f)
        , vigilance(0.8f)
        , locusCoeruleusGain(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized for arousal and vigilance");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

void Norepinephrine::increaseArousal(float amount) {
    pImpl->arousalLevel = 1.0f + amount;
}

void Norepinephrine::enhanceVigilance(float amount) {
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + amount);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for focus and learning under stress
    // Inverted U-shaped relationship: optimal NE for learning
    float optimal = 0.2f;
    float distance = std::abs(pImpl->level - optimal);
    float modulation = std::exp(-distance * 5.0f);
    
    float factor = 0.8f + modulation * 1.5f;  // Range: ~0.8 to 2.3
    return std::clamp(factor, 0.5f, 3.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: tonic and phasic signaling
    float targetLevel = pImpl->baseline;
    
    // Tonic decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small noise for phasic variations
    static float noise = 0.0f;
    noise = noise * 0.98f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
    pImpl->level += noise * 0.05f;
}

void Norepinephrine::signalStressOrNovelty(float input) {
    // LC neurons respond to novel, surprising, or stress-inducing stimuli
    if (input > 0.0f) {
        float burst = std::min(0.15f, input * pImpl->releaseRate);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Enhance vigilance
        enhanceVigilance(input * 0.3f);
        
        // Increase arousal
        increaseArousal(input * 0.2f);
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 1.0f;
    pImpl->vigilance = 0.8f;
}

} // namespace nlm