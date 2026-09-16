#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal system state
    float arousalLevel;
    float vigilanceLevel;
    float excitability;
    
    // Signal processing
    float signalStrength;
    float noiseLevel;
    float signalToNoiseRatio;
    
    // Memory and attention modulation
    float encodingBoost;
    float attentionGain;
    
    // State tracking
    float lastNovelty;
    size_t spikeCountSinceEvent;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.15f)
        , releaseRate(1.2f)
        , arousalLevel(0.0f)
        , vigilanceLevel(0.0f)
        , excitability(1.0f)
        , signalStrength(1.0f)
        , noiseLevel(0.1f)
        , signalToNoiseRatio(1.0f)
        , encodingBoost(1.0f)
        , attentionGain(1.0f)
        , lastNovelty(0.0f)
        , spikeCountSinceEvent(0) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine (NE) system initialized for arousal and vigilance");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    
    // Update derived states
    pImpl->arousalLevel = pImpl->level;
    pImpl->vigilanceLevel = pImpl->level;
    pImpl->excitability = 1.0f + pImpl->level * 0.8f; // High excitability
    pImpl->signalToNoiseRatio = 1.0f + pImpl->level * 2.0f; // Strong signal enhancement
    pImpl->encodingBoost = 1.0f + pImpl->level * 0.5f; // Better encoding
    pImpl->attentionGain = 1.0f + pImpl->level * 0.4f; // Enhanced attention
}

float Norepinephrine::getPlasticityFactor() const {
    return 1.0f + pImpl->level * 0.3f; // Moderate plasticity enhancement
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    float target = pImpl->baseline;
    pImpl->level += (target - pImpl->level) * 0.08f * static_cast<float>(dt);
    
    // Reset spike counter
    pImpl->spikeCountSinceEvent = 0;
}

void Norepinephrine::enhanceArousal(float noveltyLevel) {
    pImpl->lastNovelty = noveltyLevel;
    float arousalBurst = std::min(1.0f, pImpl->level + noveltyLevel * 0.6f * pImpl->releaseRate);
    setLevel(arousalBurst);
    
    // Update arousal state
    pImpl->arousalLevel = arousalBurst;
}

void Norepinephrine::increaseFiringThreshold(float thresholdBoost) {
    // NE actually INCREASES firing threshold (stabilizes firing)
    // But also increases signal detection
    if (pImpl->brain) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                // NE modulates neuron excitability
                neuron->injectCurrent(-thresholdBoost * 0.5f); // Slight inhibition
            }
        }
    }
}

void Norepinephrine::enhanceSignalDetection(float signalStrength) {
    pImpl->signalStrength = signalStrength;
    pImpl->noiseLevel = 0.1f * (1.0f - pImpl->level); // Noise decreases with NE
    pImpl->signalToNoiseRatio = (signalStrength + 0.1f) / (pImpl->noiseLevel + 0.01f);
}

void Norepinephrine::improveMemoryEncoding(float encodingBoost) {
    pImpl->encodingBoost = encodingBoost * pImpl->level;
    
    // Enhance memory encoding if brain and memory system available
    if (pImpl->brain && pImpl->brain->getWorkingMemory()) {
        auto* workingMemory = pImpl->brain->getWorkingMemory();
        workingMemory->strengthenMemory(pImpl->encodingBoost);
    }
}

void Norepinephrine::enhanceAttention(float focusGain) {
    pImpl->attentionGain = focusGain * pImpl->level;
    
    // Apply attentional modulation
    if (pImpl->brain && pImpl->brain->getAttention()) {
        auto* attention = pImpl->brain->getAttention();
        
        // Enhance top-down control
        attention->setExcitationStrength(1.0f + pImpl->attentionGain * 0.5f);
        attention->setInhibitionStrength(0.3f / (pImpl->attentionGain + 0.1f));
        
        // Process competition to enhance focused attention
        if (pImpl->brain->getWorkingMemory() && pImpl->brain->getWorkingMemory()->getActiveTraces() > 0) {
            std::vector<NeuronId> memoryNeurons = pImpl->brain->getWorkingMemory()->getMemoryNeurons();
            if (!memoryNeurons.empty()) {
                for (const auto& neuronId : memoryNeurons) {
                    float activation = pImpl->brain->getWorkingMemory()->getNeuronActivation(neuronId);
                    if (activation > 0.5f) {
                        attention->applyBottomUpSalience(neuronId, activation * pImpl->attentionGain);
                    }
                }
            }
        }
    }
}

float Norepinephrine::getSignalToNoiseRatio() const {
    return pImpl->signalToNoiseRatio;
}

float Norepinephrine::get alertness() const {
    return pImpl->arousalLevel;
}

} // namespace nlm
