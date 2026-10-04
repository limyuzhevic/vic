#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceModulation;
    bool isThreatSignal;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.03f), peak(1.0f), decayRate(0.08f), 
              arousalLevel(1.0f), vigilanceModulation(1.0f), isThreatSignal(false) {}
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
    // NE modulates plasticity based on arousal and threat
    return 0.4f + 0.6f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reset threat signal
    pImpl->isThreatSignal = false;
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Norepinephrine::applyToNeuralExcitability() {
    if (!pImpl->brain) return;
    
    float neLevel = getLevel();
    if (neLevel > 0.0f) {
        // NE increases overall arousal and neural excitability
        pImpl->arousalLevel = 1.0f + neLevel * 1.2f;
        pImpl->vigilanceModulation = 1.0f + neLevel * 0.5f;
        
        // Strong NE enhances internal neuron activity (arousal)
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, neLevel * 2.0f);
        
        // Moderate NE enhances sensory processing
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, neLevel * 1.2f);
    }
}

void Norepinephrine::applyToPlasticity(STDP* stdp, Hebbian* hebbian) {
    if (!stdp || !hebbian) return;
    
    float factor = getPlasticityFactor();
    
    // NE enhances plasticity during high arousal states
    float ltpWeight = stdp->getLTPWeight();
    float ltdWeight = stdp->getLTDWeight();
    
    // Boost learning during arousal
    stdp->setLTPWeight(ltpWeight * factor * 1.1f);
    stdp->setLTDWeight(ltdWeight * factor * 0.9f);
    
    // Hebbian learning is enhanced by NE for salient/important events
    if (pImpl->isThreatSignal) {
        hebbian->setLearningRate(hebbian->getLearningRate() * factor * 1.3f);
    } else {
        hebbian->setLearningRate(hebbian->getLearningRate() * factor * 1.1f);
    }
}

void Norepinephrine::applyToMemory(class NeuralEpisodicMemory* episodicMemory) {
    if (!pImpl->brain || !episodicMemory) return;
    
    float neLevel = getLevel();
    if (neLevel > 0.1f) {
        // NE enhances memory encoding during high arousal states
        float encodingFactor = 0.3f + 0.7f * neLevel;
        
        // Stronger encoding for emotionally salient events
        if (pImpl->isThreatSignal) {
            encodingFactor *= 1.5f;
        }
    }
}

void Norepinephrine::applyToAttention(class AttentionalSelection* attention) {
    if (!pImpl->brain || !attention) return;
    
    float neLevel = getLevel();
    if (neLevel > 0.0f) {
        // NE enhances attention and alertness
        attention->setExcitationStrength(attention->getExcitationStrength() * pImpl->vigilanceModulation);
        attention->setInhibitionStrength(attention->getInhibitionStrength() * 0.7f); // Reduce inhibition
        
        // If threat signal, boost attention dramatically
        if (pImpl->isThreatSignal) {
            attention->setExcitationStrength(attention->getExcitationStrength() * 1.5f);
        }
    }
}

void Norepinephrine::applyToDevelopment(class DevelopmentSystem* development) {
    if (!development) return;
    
    float neLevel = getLevel();
    if (neLevel > 0.0f) {
        // NE affects stress-related development and circuit pruning
        development->setPruningRate(development->getPruningRate() * (1.0f + neLevel * 0.3f));
    }
}

void Norepinephrine::signalStress(float stress) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * 1.0f);
    pImpl->isThreatSignal = true;
}

void Norepinephrine::enhanceVigilance(bool isThreat) {
    pImpl->isThreatSignal = isThreat;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceModulation() const {
    return pImpl->vigilanceModulation;
}

} // namespace nlm