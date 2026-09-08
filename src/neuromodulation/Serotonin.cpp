#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float mood;
    float impulsivity;
    float socialBonding;
    float decayRate;
    float responseTime;
    
    Impl() : level(0.0f), baseline(0.1f), mood(0.5f), impulsivity(0.5f),
             socialBonding(0.5f), decayRate(0.05f), responseTime(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f); // Can go up to 2.0 for high serotonergic tone
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and arousal
    // Moderate levels enhance emotional memory consolidation
    if (pImpl->level < 0.3f) {
        // Low serotonin: anxiety, high stress response
        return 1.5f + (0.3f - pImpl->level) * 0.5f;
    } else if (pImpl->level < 1.0f) {
        // Moderate levels: optimal emotional regulation and memory
        return 1.0f + (pImpl->level - 0.3f) * 0.3f;
    } else {
        // High serotonin: sedation, reduced plasticity
        return 1.2f - (pImpl->level - 1.0f) * 0.2f;
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Integrate with mood state
    pImpl->mood = 0.9f * pImpl->mood + 0.1f * pImpl->level;
    pImpl->impulsivity = std::max(0.1f, 0.5f - pImpl->level * 0.2f);
}

void Serotonin::affectAttention(class AttentionalSelection* attention) {
    if (!attention) return;
    
    // Serotonin affects attentional stability and filtering
    if (pImpl->level > 0.8f) {
        // High serotonin: calmer, more focused attention, less distractible
        attention->setExcitationStrength(1.2f);
        attention->setInhibitionStrength(0.6f);
    } else if (pImpl->level > 0.3f) {
        // Moderate serotonin: balanced attention
        attention->setExcitationStrength(1.5f);
        attention->setInhibitionStrength(0.4f);
    } else {
        // Low serotonin: distractible, anxious attention
        attention->setExcitationStrength(2.0f);
        attention->setInhibitionStrength(0.3f);
    }
}

void Serotonin::affectMemory(class NeuralWorkingMemory* workingMemory,
                              class NeuralEpisodicMemory* episodicMemory,
                              class NeuralAssociativeMemory* associativeMemory) {
    if (!workingMemory || !episodicMemory) return;
    
    // Serotonin enhances emotional memory consolidation
    float emotionalWeight = pImpl->mood;
    
    if (pImpl->level > 0.5f) {
        // High serotonin: enhanced consolidation of positive emotional memories
        episodicMemory->setConsolidationRate(1.3f + emotionalWeight * 0.2f);
        
        // Boost working memory stability
        workingMemory->setDecayRate(0.008f * (1.0f + emotionalWeight * 0.5f));
    }
}

void Serotonin::affectPlasticity(class STDP* stdp, class Hebbian* hebbian,
                                 class StructuralPlasticity* structural) {
    if (!stdp) return;
    
    // Serotonin modulates STDP for emotional memory
    if (pImpl->level > 0.5f) {
        // Enhance LTP for emotionally salient events
        float ltpMod = 1.0f + (pImpl->level - 0.5f) * 0.4f;
        stdp->setLTPWeight(stdp->getLTPWeight() * ltpMod);
        
        // Reduce LTD for emotional contexts
        float ltdMod = std::max(0.0f, 1.0f - (pImpl->level - 0.5f) * 0.3f);
        stdp->setLTDWeight(stdp->getLTDWeight() * ltdMod);
    }
}

void Serotonin::affectBehavior(class NeuralPlanner* planner) {
    if (!planner) return;
    
    // Serotonin affects impulse control and social behavior
    if (pImpl->level > 0.7f) {
        // High serotonin: more impulsive, less planning, more social exploration
        float impulsivityBoost = pImpl->impulsivity;
        planner->setPlanningDepth(std::max(static_cast<size_t>(2), planner->getPlanningDepth() - 
                                           static_cast<size_t>(impulsivityBoost * 2)));
    } else if (pImpl->level > 0.3f) {
        // Moderate serotonin: balanced behavior
        planner->setPlanningDepth(planner->getPlanningDepth());
    } else {
        // Low serotonin: anxious, risk-averse behavior
        planner->setPlanningDepth(std::min(planner->getPlanningDepth() + 2, 10u));
    }
}

void Serotonin::signalSalience(float salience) {
    // Serotonin responds to social and safety signals
    if (salience > 0.5f) {
        // Positive social signal: increase serotonin tone
        setLevel(getLevel() + salience * 0.3f);
    } else if (salience < 0.3f) {
        // Negative or neutral signal: decrease or maintain
        setLevel(getLevel() - (0.3f - salience) * 0.1f);
    }
}

} // namespace nlm
