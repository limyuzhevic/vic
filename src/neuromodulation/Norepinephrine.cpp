#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float vigilanceGain;
    float arousal;
    float stressResponse;
    float decayRate;
    float rebound;
    
    Impl() : level(0.0f), baseline(0.02f), vigilanceGain(1.5f),
             arousal(1.0f), stressResponse(0.0f), decayRate(0.03f), rebound(0.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f); // Can go up to 2.0 for high arousal
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal state
    // Moderate NE enhances plasticity, high NE can impair it
    if (pImpl->level < 0.5f) {
        return 1.0f + pImpl->level * 0.5f; // Enhances plasticity
    } else {
        return 1.5f - (pImpl->level - 0.5f) * 0.3f; // Reduces plasticity at high levels
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay with some rebound effect
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply rebound effect
    if (pImpl->rebound > 0.0f) {
        pImpl->level = std::min(pImpl->level + pImpl->rebound * 0.1f, 2.0f);
        pImpl->rebound *= 0.9f;
    }
}

void Norepinephrine::affectAttention(class AttentionalSelection* attention) {
    if (!attention) return;
    
    // NE enhances signal-to-noise ratio in attention
    float attentionGain = 1.0f + pImpl->level * 0.8f;
    
    if (pImpl->level > 1.0f) {
        // High NE: focused attention, reduced distraction
        attention->setExcitationStrength(attentionGain * 1.2f);
        attention->setInhibitionStrength(0.3f + pImpl->level * 0.1f);
    } else {
        // Moderate NE: enhanced alertness and vigilance
        attention->setExcitationStrength(attentionGain);
        attention->setInhibitionStrength(0.5f);
    }
}

void Norepinephrine::affectMemory(class NeuralWorkingMemory* workingMemory,
                                  class NeuralEpisodicMemory* episodicMemory,
                                  class NeuralAssociativeMemory* associativeMemory) {
    if (!workingMemory) return;
    
    // NE enhances memory encoding during high arousal
    float encodingBoost = 1.0f + pImpl->level * 0.3f;
    
    if (pImpl->level > 0.7f) {
        // High NE: enhanced encoding of salient events
        if (episodicMemory) {
            episodicMemory->setEncodingStrength(episodicMemory->getEncodingStrength() * 1.3f);
        }
    }
}

void Norepinephrine::affectPlasticity(class STDP* stdp, class Hebbian* hebbian,
                                      class StructuralPlasticity* structural) {
    if (!stdp) return;
    
    // NE modulates STDP based on arousal
    if (pImpl->level > 0.5f) {
        // High arousal: reduces LTD, enhances LTP
        float ltpMod = 1.0f + (pImpl->level - 0.5f) * 0.5f;
        stdp->setLTPWeight(stdp->getLTPWeight() * ltpMod);
        // Reduce LTD at high arousal
        float ltdMod = std::max(0.0f, 1.0f - (pImpl->level - 0.5f) * 0.4f);
        stdp->setLTDWeight(stdp->getLTDWeight() * ltdMod);
    }
}

void Norepinephrine::affectBehavior(class NeuralPlanner* planner) {
    if (!planner) return;
    
    // NE affects decision speed and risk aversion
    if (pImpl->level > 1.0f) {
        // High arousal: faster, more impulsive decisions
        planner->setPlanningDepth(std::max(static_cast<size_t>(2), planner->getPlanningDepth() / 2));
    } else if (pImpl->level > 0.3f) {
        // Moderate arousal: balanced decision making
        planner->setPlanningDepth(planner->getPlanningDepth());
    } else {
        // Low arousal: more thorough, cautious planning
        planner->setPlanningDepth(std::min(planner->getPlanningDepth() + 2, 10u));
    }
}

void Norepinephrine::signalArousal(float arousal) {
    // Responds to stressors and challenging situations
    if (arousal > 0.5f) {
        // Apply stress response
        setLevel(getLevel() + arousal * 0.5f);
        pImpl->rebound = arousal * 0.3f;
        
        // Stress can enhance memory consolidation
        if (pImpl->level > 1.0f) {
            // High stress: prepare for fight/flight
            pImpl->stressResponse = 1.0f;
        }
    }
}

void Norepinephrine::signalSalience(float salience) {
    // NE amplifies salient and potentially threatening stimuli
    if (salience > 0.7f) {
        // High salience: strong NE response
        setLevel(getLevel() + salience * 0.4f);
    } else {
        // Moderate salience: adjust vigilance
        setLevel(getLevel() + salience * 0.2f);
    }
}

} // namespace nlm
