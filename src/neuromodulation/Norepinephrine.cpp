#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float plasticityModulation;
    
    // Arousal and alertness
    float arousal;
    float vigilance;
    float stressLevel;
    
    Impl()
        : level(0.0f)
        , baseline(0.05f)
        , peak(2.0f)
        , decayRate(0.05f)
        , plasticityModulation(1.0f)
        , arousal(0.0f)
        , vigilance(0.0f)
        , stressLevel(0.0f) {}
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
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and vigilance
    // Affects learning and attention
    pImpl->plasticityModulation = 1.0f + pImpl->level * 0.8f;
    
    // Higher NE improves signal detection but can impair complex tasks
    return pImpl->plasticityModulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real NE dynamics with exponential decay
    if (dt > 0.0) {
        float decayFactor = std::exp(-pImpl->decayRate * dt);
        pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    }
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Arousal modulation
    pImpl->arousal = std::min(1.0f, arousalLevel);
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * 1.5f);
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    // Vigilance modulation (attention to task)
    pImpl->vigilance = std::clamp(vigilanceLevel, 0.0f, 1.0f);
    
    // High vigilance increases NE level for sustained attention
    if (vigilanceLevel > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (vigilanceLevel - 0.5f) * 2.0f);
    }
}

void Norepinephrine::signalStress(float stressLevel) {
    // Stress modulation
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    
    // Stress increases NE for fight-or-flight response
    if (stressLevel > 0.3f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (stressLevel - 0.3f) * 3.0f);
    }
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getPlasticityModulation() const {
    // Complex NE plasticity modulation based on arousal and vigilance
    float arousalFactor = pImpl->arousal * 0.4f;
    float vigilanceFactor = pImpl->vigilance * 0.3f;
    float stressFactor = pImpl->stressLevel * 0.3f;
    
    pImpl->plasticityModulation = 1.0f + arousalFactor + vigilanceFactor - stressFactor * 0.5f;
    pImpl->plasticityModulation = std::clamp(pImpl->plasticityModulation, 0.5f, 2.5f);
    
    return pImpl->plasticityModulation;
}

} // namespace nlm
