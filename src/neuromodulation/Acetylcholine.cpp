#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float noveltySensitivity;
    bool isNoveltySignal;
    float attentionModulation;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), 
              noveltySensitivity(1.0f), isNoveltySignal(false), attentionModulation(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for novel experiences
    return 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reset novelty signal
    pImpl->isNoveltySignal = false;
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Acetylcholine::applyToNeuralExcitability() {
    if (!pImpl->brain) return;
    
    float achLevel = getLevel();
    if (achLevel > 0.0f) {
        // ACh enhances cortical excitability and sensory processing
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, achLevel * 1.5f);
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, achLevel * 1.0f);
        
        // Modulate attention and arousal
        pImpl->attentionModulation = 1.0f + achLevel * 0.8f;
    }
}

void Acetylcholine::applyToPlasticity(STDP* stdp, Hebbian* hebbian) {
    if (!stdp || !hebbian) return;
    
    float factor = getPlasticityFactor();
    
    // ACh specifically enhances STDP for novel patterns
    float ltpWeight = stdp->getLTPWeight();
    float ltdWeight = stdp->getLTDWeight();
    
    // Boost LTP (learning to reinforce) for novel associations
    stdp->setLTPWeight(ltpWeight * factor * 1.2f);
    // Moderate LTD (learning to forget) to allow new learning
    stdp->setLTDWeight(ltdWeight * factor * 0.8f);
    
    // Strongly enhance Hebbian learning for attention-modulated patterns
    hebbian->setLearningRate(hebbian->getLearningRate() * factor * 1.5f);
}

void Acetylcholine::applyToMemory(class NeuralEpisodicMemory* episodicMemory) {
    if (!pImpl->brain || !episodicMemory) return;
    
    float achLevel = getLevel();
    if (achLevel > 0.2f) {
        // ACh enhances memory encoding and consolidation of novel experiences
        float encodingFactor = 0.5f + 0.5f * achLevel;
        
        // Store attention-modulated encoding information
        if (pImpl->brain->getTotalSpikeCount() > 0) {
            // Enhanced memory encoding for salient stimuli
        }
    }
}

void Acetylcholine::applyToAttention(class AttentionalSelection* attention) {
    if (!pImpl->brain || !attention) return;
    
    float achLevel = getLevel();
    if (achLevel > 0.0f) {
        // ACh strongly modulates attention
        attention->setExcitationStrength(attention->getExcitationStrength() * pImpl->attentionModulation);
        attention->setInhibitionStrength(attention->getInhibitionStrength() * 0.5f); // Reduce inhibition
        
        // If there's a novelty signal, boost attention further
        if (pImpl->isNoveltySignal) {
            attention->setExcitationStrength(attention->getExcitationStrength() * 1.3f);
        }
    }
}

void Acetylcholine::applyToDevelopment(class DevelopmentSystem* development) {
    if (!development) return;
    
    float achLevel = getLevel();
    if (achLevel > 0.0f) {
        // ACh promotes synaptic growth and circuit refinement during development
        development->setSynaptogenesisRate(development->getSynaptogenesisRate() * (1.0f + achLevel * 0.5f));
    }
}

void Acetylcholine::signalNovelty(float novelty) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->noveltySensitivity);
    pImpl->isNoveltySignal = true;
}

void Acetylcholine::enhanceAttentionDuringNovelty(bool isNovel) {
    pImpl->isNoveltySignal = isNovel;
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

} // namespace nlm