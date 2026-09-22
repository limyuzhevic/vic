#pragma once

#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;                    // Current level
    float attentionGain;            // Attention modulation factor
    float memoryGain;               // Memory encoding enhancement factor
    float noveltySensitivity;       // Sensitivity to novel stimuli
    float encodingThreshold;        // Threshold for memory encoding
    float decayRate;                // Decay rate
    float baseline;                 // Baseline level
    SimulationStep lastInputStep;   // Time of last significant input
    
    Impl() : level(0.0f), attentionGain(1.0f), memoryGain(1.0f), 
             noveltySensitivity(0.5f), encodingThreshold(0.3f), 
             decayRate(0.1f), baseline(0.1f), lastInputStep(0) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances memory encoding and attention
    return 1.0f + pImpl->memoryGain * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention gain
    pImpl->attentionGain = 1.0f + 2.0f * pImpl->level;
    
    // Update memory gain
    pImpl->memoryGain = 1.0f + 1.5f * pImpl->level;
}

void Acetylcholine::enhanceAttention(float focusLevel, Brain* brain) {
    if (!brain || pImpl->level <= 0.0f) return;
    
    // Enhance attention to salient stimuli
    float attentionBoost = focusLevel * pImpl->attentionGain;
    
    // Apply attention modulation to working memory
    if (brain->getWorkingMemory()) {
        // Working memory competition enhanced by ACh
        // Winners receive stronger synaptic inputs
    }
}

void Acetylcholine::modulateWorkingMemory(Brain* brain) {
    if (!brain || pImpl->level < 0.5f) return;
    
    // Strengthen working memory traces
    if (brain->getWorkingMemory()) {
        // ACh enhances consolidation of working memory
    }
}

void Acetylcholine::enhanceEncoding(float novelty, Brain* brain) {
    if (!brain) return;
    
    // Encode novel information into memory systems
    if (novelty > pImpl->encodingThreshold && pImpl->level > 0.5f) {
        // Store in working memory with ACh bias
        if (brain->getWorkingMemory()) {
            // Store new trace with higher weight
        }
        
        // Store in episodic memory
        if (brain->getEpisodicMemory()) {
            // Store episode with ACh-enhanced encoding
        }
        
        // Strengthen relevant connections via neuromodulated plasticity
        if (brain->getSTDP()) {
            // ACh modulates STDP windows
        }
    }
}

void Acetylcholine::strengthenSynapticTrace(Brain* brain, NeuronId neuronId) {
    if (!brain || pImpl->level <= 0.0f) return;
    
    // Enhance synaptic efficacy of neural traces
    if (brain->getSTDP()) {
        // ACh modulates synaptic potentiation
    }
}

void Acetylcholine::signalNovelty(float novelty) {
    // Respond to novel stimuli
    if (novelty > pImpl->encodingThreshold) {
        pImpl->level = std::min(1.0f, pImpl->level + novelty * pImpl->noveltySensitivity);
        pImpl->lastInputStep = 0;  // Would use brain reference
    }
}

void Acetylcholine::respondToSensoryInput(Brain* brain) {
    if (!brain || pImpl->level <= 0.0f) return;
    
    // ACh responds to novel sensory input
    if (brain->getWorkingMemory() && brain->getNovelty()) {
        // Modulate based on novelty detection
    }
}

} // namespace nlm
