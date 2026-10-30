#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float arousal;
    float vigilance;
    float explorationBias;
    float stressLevel;
    float decayRate;
    SimulationStep lastUpdate;
    std::vector<NeuronId> highVigilanceNeurons;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.03f)
        , arousal(1.0f)
        , vigilance(0.5f)
        , explorationBias(0.0f)
        , stressLevel(0.0f)
        , decayRate(0.04f)
        , lastUpdate(0) {}
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
    // Norepinephrine modulates plasticity: enhances learning during high arousal
    float plasticity = 0.5f + 0.5f * pImpl->level;
    
    // Add stress modulation
    if (pImpl->stressLevel > 0.7f) {
        // Stress reduces plasticity (too much arousal impairs learning)
        plasticity *= 0.7f;
    }
    
    return std::clamp(plasticity, 0.0f, 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Update vigilance and arousal based on environment
    pImpl->arousal = pImpl->level * 2.0f;  // Arousal scales with NE level
    pImpl->vigilance = 0.5f + 0.5f * pImpl->level;  // Vigilance increases with NE
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Stress adaptation
    if (pImpl->stressLevel > 0.5f) {
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.01f);
    }
    
    // Apply arousal effects on neural excitability
    float excitabilityMod = pImpl->arousal * 0.3f;
    if (excitabilityMod > 0.0f) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE increases firing threshold for sustained attention
                    if (pImpl->level > 0.3f) {
                        // Increase resting potential to require more input
                        float current = neuron->getState().membranePotential;
                        neuron->setMembranePotential(std::min(40.0f, current + 0.5f));
                    }
                }
            }
        }
    }
    
    // Enhanced detection of salient stimuli
    updateVigilanceEffects(dt);
    
    // Log high-level status periodically
    pImpl->lastUpdate++;
    if (pImpl->lastUpdate % 100 == 0) {
        NLM_LOG_INFO("Norepinephrine: Level=" + std::to_string(pImpl->level) +
                    " Arousal=" + std::to_string(pImpl->arousal) +
                    " Vigilance=" + std::to_string(pImpl->vigilance));
    }
}

void Norepinephrine::updateVigilanceEffects(TimestepDuration dt) {
    // Select high-vigilance neurons for enhanced processing
    if (pImpl->vigilance > 0.7f && pImpl->brain) {
        // High vigilance: enhance sensory processing
        float vigilanceBoost = (pImpl->vigilance - 0.7f) * 3.0f;
        
        // Boost sensory neuron excitability
        for (auto* neuron : pImpl->sensoryNeurons) {
            neuron->injectCurrent(vigilanceBoost * 2.0f);
        }
        
        // Update high vigilance neurons list
        pImpl->highVigilanceNeurons.clear();
        for (size_t i = 0; i < pImpl->sensoryNeurons.size() && i < 5; ++i) {
            pImpl->highVigilanceNeurons.push_back(pImpl->sensoryNeurons[i]->getId());
        }
    }
}

void Norepinephrine::signalArousal(float intensity) {
    // Signal arousal (e.g., due to novel stimulus)
    pImpl->level = std::min(1.0f, pImpl->level + intensity * 0.3f);
    
    // Log arousal signal
    NLM_LOG_INFO("Norepinephrine: Arousal signal detected, level=" + std::to_string(pImpl->level));
}

void Norepinephrine::signalStress(float stressLevel) {
    // Signal stress (negative stimulus)
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stressLevel * 0.2f);
    
    // Stress reduces learning efficiency
    pImpl->level = std::max(pImpl->baseline, pImpl->level - stressLevel * 0.2f);
    
    // Log stress signal
    NLM_LOG_INFO("Norepinephrine: Stress signal detected, level=" + std::to_string(pImpl->level) +
                " Stress=" + std::to_string(pImpl->stressLevel));
}

void Norepinephrine::enhanceExploration(float explorationAmount) {
    // Increase exploration bias
    pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + explorationAmount);
    
    // Exploration is encoded as increased motor neuron activity
    if (pImpl->brain && pImpl->brain->getMotorSystem()) {
        pImpl->brain->getMotorSystem()->activateRandomActions(explorationAmount);
    }
    
    NLM_LOG_INFO("Norepinephrine: Exploration enhanced, bias=" + std::to_string(pImpl->explorationBias));
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getExplorationBias() const {
    return pImpl->explorationBias;
}

bool Norepinephrine::isHighVigilance() const {
    return pImpl->vigilance > 0.7f;
}

} // namespace nlm
