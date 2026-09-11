#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalLevel;
    float vigilance;
    float locusCoeruleusGain;
    float baselineLevel;
    float peakLevel;
    float decayRate;
    float releaseRate;
    
    // Alertness parameters
    float signalDetection;
    float phasicResponse;
    float tonicResponse;
    
    // Plasticity modulation
    float ltpBoost;
    float ltdBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalLevel(1.0f)
        , vigilance(1.0f)
        , locusCoeruleusGain(1.0f)
        , baselineLevel(0.0f)
        , peakLevel(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , signalDetection(1.0f)
        , phasicResponse(1.0f)
        , tonicResponse(1.0f)
        , ltpBoost(1.0f)
        , ltdBoost(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine (arousal/vigilance) system initialized");
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
    // NE modulates both LTP and LTD
    float plasticity = 1.0f;
    if (pImpl->level > 0.5f) {
        // High NE enhances LTP
        plasticity = 1.2f + (pImpl->level - 0.5f) * 0.5f;
    } else if (pImpl->level < 0.3f) {
        // Low NE enhances LTD
        plasticity = 0.8f - (0.3f - pImpl->level) * 0.4f;
    }
    return plasticity;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baselineLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal and vigilance
    pImpl->arousalLevel = 1.0f + pImpl->level * 2.0f;  // 1.0x to 3.0x arousal
    pImpl->vigilance = 1.0f + pImpl->level * 1.5f;      // 1.0x to 2.5x vigilance
    
    // NE effects on neural signaling
    if (pImpl->brain) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE enhances signal detection
                    float signalEnhance = pImpl->level * 0.4f;
                    if (signalEnhance > 0.0f) {
                        neuron->injectCurrent(signalEnhance);
                    }
                    
                    // Tonic and phasic firing modulation
                    if (pImpl->tonicResponse > 1.0f) {
                        // Increase baseline firing probability
                        neuron->setThreshold(neuron->getState().threshold * 0.9f);
                    }
                }
            }
        }
    }
}

void Norepinephrine::signalAlert(float alertStrength) {
    // Phasic NE response to novel or important stimuli
    setLevel(std::min(pImpl->peakLevel, getLevel() + alertStrength * pImpl->releaseRate));
    pImpl->phasicResponse = 1.0f + getLevel();
    pImpl->signalDetection = 1.0f + getLevel() * 2.0f;
    
    NLM_LOG_INFO("Norepinephrine phasic response to alert: level=" + std::to_string(getLevel()));
}

void Norepinephrine::increaseArousal(float arousalSignal) {
    // Tonic NE for sustained arousal
    setLevel(std::min(pImpl->peakLevel, getLevel() + arousalSignal * 0.5f));
    pImpl->tonicResponse = 1.0f + getLevel();
    
    NLM_LOG_INFO("Norepinephrine increased arousal to " + std::to_string(getLevel()));
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getSignalDetection() const {
    return pImpl->signalDetection;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->arousalLevel = 1.0f;
    pImpl->vigilance = 1.0f;
    pImpl->locusCoeruleusGain = 1.0f;
    pImpl->signalDetection = 1.0f;
    pImpl->phasicResponse = 1.0f;
    pImpl->tonicResponse = 1.0f;
    pImpl->ltpBoost = 1.0f;
    pImpl->ltdBoost = 1.0f;
}

} // namespace nlm