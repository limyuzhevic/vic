#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Neuromodulatory effects
    float excitationModulation;     // Modulates neural excitability
    float plasticityModulation;     // Modulates synaptic plasticity
    float memoryConsolidation;      // Modulates memory consolidation
    float attentionGain;            // Modulates attentional gain
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
            , excitationModulation(1.0f), plasticityModulation(1.0f)
            , memoryConsolidation(1.0f), attentionGain(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update derived neuromodulatory effects
    updateModulatoryEffects();
}

float Dopamine::getPlasticityFactor() const {
    // Returns overall plasticity modulation factor
    return pImpl->plasticityModulation;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // Placeholder: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateModulatoryEffects();
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // Placeholder: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    updateModulatoryEffects();
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // Placeholder: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    updateModulatoryEffects();
}

void Dopamine::updateModulatoryEffects() {
    // Update all neuromodulatory effects based on current dopamine level
    // This is the key improvement - dopamine affects multiple systems
    
    // 1. Excitation modulation - dopamine increases neural excitability
    pImpl->excitationModulation = 0.5f + pImpl->level * 0.5f;  // 0.5 to 1.0
    
    // 2. Plasticity modulation - dopamine gates plasticity
    // Phasic bursts strongly increase plasticity
    pImpl->plasticityModulation = 0.5f + pImpl->level * 0.5f;
    
    // 3. Memory consolidation - dopamine enhances consolidation during high states
    pImpl->memoryConsolidation = 0.5f + pImpl->level * 0.5f;
    
    // 4. Attention gain - dopamine improves signal-to-noise ratio
    pImpl->attentionGain = 0.5f + pImpl->level * 0.5f;
}

float Dopamine::getExcitationModulation() const {
    return pImpl->excitationModulation;
}

float Dopamine::getMemoryConsolidationFactor() const {
    return pImpl->memoryConsolidation;
}

float Dopamine::getAttentionGain() const {
    return pImpl->attentionGain;
}

void Dopamine::applyToNeuron(Neuron* neuron, TimestepDuration dt) {
    // Apply dopamine effects to a single neuron
    if (neuron && pImpl->level > 0.0f) {
        // 1. Increase excitability by injecting current
        float current = (pImpl->level - 0.5f) * 2.0f;  // 0 to 1.0 current
        neuron->injectCurrent(current);
        
        // 2. Modify threshold based on attention gain
        float thresholdMod = (pImpl->attentionGain - 1.0f) * 0.1f;  // -0.05 to 0
        float newThreshold = neuron->getThreshold() + thresholdMod;
        neuron->setThreshold(newThreshold);
        
        // 3. Store dopamine level in neuron for later use
        neuron->setDopamineLevel(pImpl->level);
    }
}

void Dopamine::applyToSynapse(Synapse* synapse, float predictionError, TimestepDuration dt) {
    // Apply dopamine effects to a synapse
    if (synapse && pImpl->level > 0.0f) {
        // 1. Modulate synaptic efficacy based on prediction error
        float efficacyMod = pImpl->level * predictionError;
        float newEfficacy = synapse->getEfficacy() + efficacyMod;
        synapse->setEfficacy(newEfficacy);
        
        // 2. Enhance plasticity for predicted errors
        if (predictionError != 0.0f) {
            float plasticityBoost = pImpl->level * std::abs(predictionError) * 0.1f;
            synapse->enhancePlasticity(plasticityBoost);
        }
    }
}

void Dopamine::applyToMemory(NeuralEpisodicMemory* memory, float reward, TimestepDuration dt) {
    // Apply dopamine effects to memory systems
    if (memory && pImpl->level > 0.0f) {
        // 1. Enhance memory consolidation for recent rewards
        memory->boostRelevance(reward * pImpl->level);
        
        // 2. Store dopamine state with memory episodes
        // In real implementation, this would store the neuromodulatory context
    }
}
