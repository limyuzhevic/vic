#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm
{

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float riseRate;
    float arousalLevel;
    float uncertaintyModulationFactor;
    float stressSignals[2]; // uncertainty, arousal
    std::vector<float> firingPatternHistory;
    std::vector<float> vigilanceHistory;
    std::mt19937 rng;
    
    Impl() : brain(nullptr), level(0.01f), baseline(0.001f), peak(1.0f), decayRate(0.03f), 
              riseRate(3.0f), arousalLevel(0.0f), uncertaintyModulationFactor(1.0f) {
        std::random_device rd;
        rng.seed(rd());
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(class Brain* brain) {
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates neuron excitability, affecting learning
    float basePlasticity = 0.5f;
    float modulation = pImpl->level * 0.4f; // Up to 1.4x baseline under high NE
    return basePlasticity + modulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal level (slower dynamics)
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + (pImpl->level - pImpl->arousalLevel) * 0.1f);
    
    // Update uncertainty modulation factor
    pImpl->uncertaintyModulationFactor = 1.0f + pImpl->level * 0.8f;
    
    // Decay history
    if (!pImpl->vigilanceHistory.empty()) {
        pImpl->vigilanceHistory.erase(pImpl->vigilanceHistory.begin());
    }
    pImpl->vigilanceHistory.push_back(pImpl->arousalLevel);
}

void Norepinephrine::signalStressEvent(float uncertainty, float arousalLevel) {
    // NE responds to stressful or uncertain situations
    float stressMagnitude = uncertainty * 0.7f + arousalLevel * 0.3f;
    
    // Bursty release in response to stress/uncertainty
    float burstMagnitude = stressMagnitude * pImpl->riseRate * static_cast<float>(0.15);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    
    // Store stress signals for memory
    pImpl->stressSignals[0] = uncertainty;
    pImpl->stressSignals[1] = arousalLevel;
    
    NLM_LOG_DEBUG("Norepinephrine signaling stress event: uncertainty=%.3f, arousal=%.3f", uncertainty, arousalLevel);
}

void Norepinephrine::modulateExcitability(class Neuron* neuron, float currentLevel) {
    if (neuron == nullptr) return;
    
    // NE modulates neuron excitability
    float excitabilityMod = pImpl->level * 0.3f * currentLevel;
    
    // Modulate neuron's resting potential or threshold
    // Higher NE increases excitability (lowers effective threshold)
    if (excitabilityMod > 0.0f) {
        neuron->injectCurrent(excitabilityMod);
    }
    
    NLM_LOG_DEBUG("Norepinephrine modulated neuron excitability by: %.3f", excitabilityMod);
}

void Norepinephrine::enhanceMemoryUnderUncertainty(const std::vector<float>& pattern, float uncertainty) {
    if (pattern.empty()) return;
    
    // NE enhances memory encoding under uncertain conditions
    float encodingBoost = pImpl->level * uncertainty * pImpl->uncertaintyModulationFactor;
    
    // Store pattern with NE-enhanced uncertainty weighting
    for (size_t i = 0; i < std::min(pattern.size(), static_cast<size_t>(1000)); ++i) {
        float enhancedValue = pattern[i] * (1.0f + encodingBoost);
        pImpl->firingPatternHistory.push_back(enhancedValue);
    }
    
    NLM_LOG_DEBUG("Norepinephrine enhanced memory encoding under uncertainty: boost=%.3f", encodingBoost);
}

void Norepinephrine::boostVigilance(float vigilanceLevel) {
    // Increase vigilance level
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + vigilanceLevel * pImpl->riseRate * 0.02f);
    
    // Can boost NE level as well
    if (vigilanceLevel > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceLevel * 0.15f);
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 0.0f;
    pImpl->uncertaintyModulationFactor = 1.0f;
    pImpl->stressSignals[0] = 0.0f;
    pImpl->stressSignals[1] = 0.0f;
    pImpl->firingPatternHistory.clear();
    pImpl->vigilanceHistory.clear();
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getUncertaintyModulationFactor() const {
    return pImpl->uncertaintyModulationFactor;
}

} // namespace nlm