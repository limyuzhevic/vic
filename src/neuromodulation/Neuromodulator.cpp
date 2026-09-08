#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float predictionBaseline;
    float learningRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionError(0.0f), predictionBaseline(0.0f), learningRate(0.01f) {}
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
}

float Dopamine::getPlasticityFactor() const {
    // Neuroscience-based dopamine modulation of plasticity
    // Phasic bursts (high prediction error) enhance learning, tonic (stable) modulates baseline plasticity
    float predictionErrorSignal = std::abs(pImpl->predictionError);
    float baselineLevel = std::max(pImpl->baseline, pImpl->level);
    
    // Strong prediction error causes high plasticity
    float plasticityFromError = predictionErrorSignal * 0.8f;
    
    // Baseline dopamine provides moderate plasticity
    float plasticityFromBaseline = baselineLevel * 0.2f;
    
    // Dopamine's inverted-U effect on plasticity (optimal at moderate levels)
    float levelEffect = 1.0f - std::abs(pImpl->level - 0.5f) * 2.0f;
    
    return 0.3f + (plasticityFromError + plasticityFromBaseline) * levelEffect;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay with prediction error integration
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Integrate prediction error over time (learning)
    if (std::abs(pImpl->predictionError) > 0.01f) {
        float integration = pImpl->predictionError * pImpl->learningRate * static_cast<float>(dt);
        pImpl->level = std::max(0.0f, pImpl->level + integration);
        pImpl->level = std::min(pImpl->peak, pImpl->level);
    }
}

void Dopamine::signalReward(float reward) {
    // Reward prediction error: reward received - expected reward
    float expected = 0.0f; // In a full implementation, would use prediction system
    float error = reward - expected;
    
    if (error > 0.0f) {
        // Positive prediction error: reward prediction burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.5f);
        pImpl->predictionError = error;
        NLM_LOG_INFO("Dopamine: Positive prediction error (reward)");
    } else {
        // Negative prediction error: prediction correction
        pImpl->predictionError = error;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Directly set prediction error for neuromodulation
    pImpl->predictionError = error;
    
    // Immediate response based on error
    if (error > 0.0f) {
        // Strong burst on positive error
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 2.0f);
        NLM_LOG_INFO("Dopamine: Reward prediction error signal");
    } else {
        // Phasic dip on negative error
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.5f);
    }
}

void Dopamine::affectAttention(class AttentionalSelection* attention) {
    if (!attention) return;
    
    // Dopamine enhances attention to rewarding stimuli
    float rewardBias = pImpl->level * 0.7f;
    
    if (pImpl->level > 0.5f) {
        // High dopamine: enhanced salience for reward-predictive cues
        attention->setExcitationStrength(1.5f + rewardBias * 0.5f);
        attention->setInhibitionStrength(0.3f - rewardBias * 0.2f);
    }
}

void Dopamine::affectMemory(class NeuralWorkingMemory* workingMemory,
                            class NeuralEpisodicMemory* episodicMemory,
                            class NeuralAssociativeMemory* associativeMemory) {
    if (!workingMemory || !episodicMemory) return;
    
    // Dopamine modulates memory consolidation and strength
    float memoryModulation = pImpl->level * 0.8f;
    
    // Enhance working memory updating with reward value
    workingMemory->setDecayRate(0.01f * (1.0f - memoryModulation));
    
    // Promote reward-related memory consolidation
    if (pImpl->level > 0.3f) {
        episodicMemory->setConsolidationRate(1.2f + memoryModulation * 0.3f);
    }
}

void Dopamine::affectPlasticity(class STDP* stdp, class Hebbian* hebbian,
                               class StructuralPlasticity* structural) {
    if (!stdp) return;
    
    // Dopamine specifically modulates STDP for reward learning
    float ltpMod = 1.0f + pImpl->level * 0.5f;
    stdp->setLTPWeight(stdp->getLTPWeight() * ltpMod);
    
    // Adjust timing windows based on dopamine level
    if (pImpl->level > 0.7f) {
        // High dopamine: broader temporal window for STDP
        // (in a full implementation, would expose timing window parameters)
    }
}

void Dopamine::affectBehavior(class NeuralPlanner* planner) {
    if (!planner) return;
    
    // Dopamine influences action selection and value-based planning
    if (pImpl->level > 0.5f) {
        // High dopamine: more exploratory, higher planning depth
        planner->setPlanningDepth(std::min(planner->getPlanningDepth() + 2, 10u));
    } else {
        // Low dopamine: more habitual, less planning
        planner->setPlanningDepth(std::max(static_cast<size_t>(2), planner->getPlanningDepth() / 2));
    }
}

} // namespace nlm
