#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalWeight;
    float vigilanceWeight;
    float memoryEnhancementWeight;
    float decayRate;
    float recoveryRate;
    float attentionThreshold;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalWeight(1.0f)
        , vigilanceWeight(1.0f)
        , memoryEnhancementWeight(0.5f)
        , decayRate(0.15f)
        , recoveryRate(0.05f)
        , attentionThreshold(0.3f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE increases arousal, vigilance, and memory encoding
    if (pImpl->brain) {
        // Enhance arousal and alertness
        if (pImpl->brain->getAttention()) {
            // NE increases attentional vigilance and signal detection
            float vigilanceBoost = std::min(1.0f, pImpl->level * 2.0f);
            pImpl->brain->getAttention()->setVigilance(vigilanceBoost);
        }
        
        // Enhance memory encoding (especially for novel events)
        if (pImpl->brain->getEpisodicMemory()) {
            // NE enhances encoding of new memories
            pImpl->brain->getEpisodicMemory()->enhanceEncoding(pImpl->level * 0.8f);
        }
        
        // Increase neural signal-to-noise ratio
        for (const auto& region : pImpl->brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                // NE increases neural responsiveness and firing reliability
                if (pImpl->level > attentionThreshold) {
                    float signalBoost = pImpl->level * 3.0f;
                    neuron->injectCurrent(signalBoost);
                }
            }
        }
        
        // Response to stress/challenge
        if (pImpl->level > 0.5f) {
            // High NE increases plasticity for learning from challenging situations
            pImpl->brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            
            // Enhance structural plasticity under stress
            if (auto* sp = pImpl->brain->getStructuralPlasticity()) {
                float stressPlasticityBoost = pImpl->level * 0.5f;
                sp->setSynaptogenesisRate(0.0001f * (1.0f + stressPlasticityBoost));
                sp->setPruningRate(0.00001f * (2.0f - stressPlasticityBoost));
            }
        }
    }
    
    // Natural dynamics: decay and slow recovery
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = std::min(1.0f, pImpl->level + pImpl->recoveryRate * static_cast<float>(dt));
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity, especially under arousal
    return 1.2f - 0.4f * pImpl->level;  // Range: [0.8, 1.2]
}

void Norepinephrine::respondToStress(float stressLevel, TimestepDuration dt) {
    // NE spikes in response to stress/challenge
    float stressResponse = std::min(1.0f, stressLevel * 3.0f);
    pImpl->level = std::min(1.0f, pImpl->level + stressResponse * 0.5f);
}

void Norepinephrine::enhanceVigilance(float vigilanceBoost, TimestepDuration dt) {
    // Increase alertness and attentiveness
    pImpl->level = std::min(1.0f, pImpl->level + vigilanceBoost * 0.3f);
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm