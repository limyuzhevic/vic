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
    
    // Arousal and vigilance parameters
    float arousalLevel;
    float vigilanceLevel;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f),
             arousalLevel(1.0f), vigilanceLevel(1.0f) {}
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
    // Norepinephrine modulates arousal and signal detection
    float arousalModulation = 1.0f;
    
    // NE enhances neural gain and signal-to-noise ratio
    arousalModulation *= (1.0f + pImpl->level * 0.8f);
    
    return std::clamp(arousalModulation, 0.5f, 3.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Implement noradrenergic dynamics
    // NE fluctuates with arousal and environmental demands
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add stress/arousal fluctuations
    float arousalMod = std::sin(static_cast<float>(pImpl->rng.generate()) * 2.0f) * 0.08f;
    pImpl->level += arousalMod;
}

void Norepinephrine::signalStress(float stressLevel) {
    // Signal stress and environmental challenges with noradrenergic responses
    float stressResponse = stressLevel * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressResponse);
    
    // Apply to sensory processing
    if (pImpl->brain) {
        auto* spikeSystem = pImpl->brain->getSpikeSystem();
        if (spikeSystem) {
            // NE enhances spike probability and sensory sensitivity
            spikeSystem->setSensitivity(spikeSystem->getSensitivity() + pImpl->level * 0.3f);
        }
    }
}

void Norepinephrine::signalAlertness(float alertness) {
    // Signal alertness and attention demands
    float alertSignal = alertness * pImpl->releaseRate * 0.7f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + alertSignal);
    
    // Enhance signal detection and reaction time
    if (pImpl->brain) {
        auto* spikeSystem = pImpl->brain->getSpikeSystem();
        if (spikeSystem) {
            float sensitivity = spikeSystem->getSensitivity();
            spikeSystem->setSensitivity(sensitivity * (1.0f + pImpl->level * 0.5f));
        }
    }
}

void Norepinephrine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getArousalLevel() const {
    return 1.0f + pImpl->level * pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return 1.0f + pImpl->level * pImpl->vigilanceLevel;
}

} // namespace nlm
