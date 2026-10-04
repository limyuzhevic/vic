#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float socialModulation;
    float behavioralControl;
    bool isSocialFeedback;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.02f), peak(1.0f), decayRate(0.06f),
              socialModulation(1.0f), behavioralControl(1.0f), isSocialFeedback(false) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and behavioral state
    return 0.2f + 0.8f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reset social feedback signal
    pImpl->isSocialFeedback = false;
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Serotonin::applyToNeuralExcitability() {
    if (!pImpl->brain) return;
    
    float serotoninLevel = getLevel();
    if (serotoninLevel > 0.0f) {
        // Serotonin modulates mood-related neural excitability
        // Generally reduces impulsivity and stabilizes firing
        pImpl->behavioralControl = 1.0f + serotoninLevel * 0.5f;
        
        // Moderate effect on internal neurons for mood regulation
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, serotoninLevel * 0.5f);
        
        // Reduce excessive excitability in sensory neurons to prevent overstimulation
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, -serotoninLevel * 0.3f);
    }
}

void Serotonin::applyToPlasticity(STDP* stdp, Hebbian* hebbian) {
    if (!stdp || !hebbian) return;
    
    float factor = getPlasticityFactor();
    
    // Serotonin modulates plasticity based on behavioral state
    float ltpWeight = stdp->getLTPWeight();
    float ltdWeight = stdp->getLTDWeight();
    
    // Higher serotonin stabilizes plasticity (prevents runaway learning)
    stdp->setLTPWeight(ltpWeight * (0.8f + 0.2f * factor));
    stdp->setLTDWeight(ltdWeight * (0.9f - 0.1f * factor));
    
    // Hebbian learning is modulated by serotonin
    if (pImpl->isSocialFeedback) {
        // Social feedback enhances appropriate learning
        hebbian->setLearningRate(hebbian->getLearningRate() * factor * 0.9f);
    } else {
        // Baseline learning rate
        hebbian->setLearningRate(hebbian->getLearningRate() * factor * 1.0f);
    }
}

void Serotonin::applyToMemory(class NeuralEpisodicMemory* episodicMemory) {
    if (!pImpl->brain || !episodicMemory) return;
    
    float serotoninLevel = getLevel();
    if (serotoninLevel > 0.1f) {
        // Serotonin enhances memory consolidation and regulatory functions
        float consolidationFactor = 0.5f + 0.5f * serotoninLevel;
        
        // Better regulation of memory retrieval and inhibition
        if (pImpl->isSocialFeedback) {
            consolidationFactor *= 1.2f;  // Enhanced social memory processing
        }
    }
}

void Serotonin::applyToAttention(class AttentionalSelection* attention) {
    if (!pImpl->brain || !attention) return;
    
    float serotoninLevel = getLevel();
    if (serotoninLevel > 0.0f) {
        // Serotonin modulates attention and impulse control
        attention->setExcitationStrength(attention->getExcitationStrength() * pImpl->behavioralControl);
        attention->setInhibitionStrength(attention->getInhibitionStrength() * (1.0f + serotoninLevel * 0.3f));
        
        // Enhanced behavioral control
        if (pImpl->isSocialFeedback) {
            attention->setExcitationStrength(attention->getExcitationStrength() * 0.9f); // More selective attention
        }
    }
}

void Serotonin::applyToDevelopment(class DevelopmentSystem* development) {
    if (!development) return;
    
    float serotoninLevel = getLevel();
    if (serotoninLevel > 0.0f) {
        // Serotonin affects behavioral development and social circuit refinement
        development->setSynaptogenesisRate(development->getSynaptogenesisRate() * (1.0f - serotoninLevel * 0.2f));
        development->setPruningRate(development->getPruningRate() * (1.0f + serotoninLevel * 0.3f));
    }
}

void Serotonin::signalSocialFeedback(float socialFeedback) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialFeedback * 0.5f);
    pImpl->isSocialFeedback = true;
}

void Serotonin::modulateImpulsivity(float impulsivity) {
    // Reduces impulsivity based on serotonin level
    if (impulsivity > 0.5f) {
        float reduction = pImpl->level * 0.3f;
        // This would affect neural firing patterns in the brain
    }
}

void Serotonin::regulateMood(float mood) {
    // Modulates mood-related neural circuits
    if (mood < 0.0f) {  // Negative mood
        pImpl->level = std::max(pImpl->peak * 0.3f, pImpl->level - 0.1f);
    } else {  // Positive mood
        pImpl->level = std::min(pImpl->peak, pImpl->level + mood * 0.1f);
    }
}

float Serotonin::getSocialModulation() const {
    return pImpl->socialModulation;
}

float Serotonin::getBehavioralControl() const {
    return pImpl->behavioralControl;
}

} // namespace nlm