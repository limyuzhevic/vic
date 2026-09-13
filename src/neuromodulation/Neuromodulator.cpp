#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // Higher dopamine increases plasticity for learning
    return 0.3f + 0.7f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline with stochastic fluctuations
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Add small noise
    pImpl->level += (rand() / (RAND_MAX * 100.0f)) * pImpl->decayRate * static_cast<float>(dt);
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to reward prediction error
    if (error > 0.5f) {
        // Positive prediction error (better than expected)
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 0.3f);
    } else if (error < -0.5f) {
        // Negative prediction error (worse than expected)
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate * 0.5f);
    }
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float decayRate;
    float attentionModulator;
    float memoryEnhancer;
    class Brain* brain;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.05f), attentionModulator(1.0f), memoryEnhancer(1.0f), brain(nullptr) {}
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
    // ACh enhances synaptic plasticity for attention and memory
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay slowly
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update brain reference if set
    if (pImpl->brain && pImpl->level > 0.5f) {
        // Enhance attention and memory consolidation
        modulateAttention(pImpl->attentionModulator);
        enhanceMemoryConsolidation();
    }
}

void Acetylcholine::modulateAttention(float attentionStrength) {
    if (!pImpl->brain) return;
    
    // Enhance attentional selection
    if (auto* attention = pImpl->brain->getAttention()) {
        // ACh increases attention to salient stimuli
        attention->setSelectionStrength(pImpl->attentionModulator * attentionStrength);
    }
}

void Acetylcholine::enhanceMemoryConsolidation() {
    if (!pImpl->brain) return;
    
    // Strengthen memory consolidation
    if (auto* episodicMemory = pImpl->brain->getEpisodicMemory()) {
        // Enhance memory consolidation rate
        episodicMemory->setConsolidationRate(pImpl->memoryEnhancer);
    }
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    float arousalLevel;
    float vigilanceBoost;
    float attentionBroadening;
    class Brain* brain;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.08f), arousalLevel(1.0f), vigilanceBoost(1.0f), attentionBroadening(1.0f), brain(nullptr) {}
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
    // NE enhances plasticity for alertness and learning
    float vigilanceMod = pImpl->vigilanceBoost * pImpl->level;
    return 0.4f + 0.6f * std::min(1.0f, vigilanceMod);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay with slower decay than other neuromodulators
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 0.5f);
    
    // Update brain reference if set
    if (pImpl->brain) {
        modulateArousal(pImpl->arousalLevel);
        enhanceVigilance(pImpl->vigilanceBoost);
    }
}

void Norepinephrine::modulateArousal(float arousalLevel) {
    if (!pImpl->brain) return;
    
    // Increase overall arousal and alertness
    pImpl->brain->injectCurrentToNeurons(NeuronType::Excitatory, 2.0f * pImpl->level);
}

void Norepinephrine::enhanceVigilance(float vigilanceBoost) {
    if (!pImpl->brain) return;
    
    // Enhance vigilance by increasing firing thresholds
    // Less threshold means more neurons fire
    float vigilanceMod = pImpl->vigilanceBoost * pImpl->level;
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Reduce threshold to enhance responsiveness
                neuron->setThreshold(0.3f - vigilanceMod * 0.2f);
            }
        }
    }
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    float moodLevel;
    float impulsivityReduction;
    float socialBehaviorFactor;
    class Brain* brain;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.06f), moodLevel(1.0f), impulsivityReduction(1.0f), socialBehaviorFactor(1.0f), brain(nullptr) {}
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
    // Serotonin modulates plasticity based on mood
    float moodMod = pImpl->moodLevel * pImpl->level;
    // Lower mood reduces plasticity, higher mood enhances it
    return 0.5f + 0.5f * (1.0f - moodMod);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay with medium rate
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update brain reference if set
    if (pImpl->brain) {
        modulateMood(pImpl->moodLevel);
        reduceImpulsivity(pImpl->impulsivityReduction);
        influenceSocialBehavior(pImpl->socialBehaviorFactor);
    }
}

void Serotonin::modulateMood(float moodLevel) {
    if (!pImpl->brain) return;
    
    // Improve mood by reducing stress response
    // Modulate inhibitory synapses
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getWeight() < 0) {  // Inhibitory synapses
                // Make them less strong
                syn->setWeight(syn->getWeight() * (1.0f + pImpl->level * 0.1f));
            }
        }
    }
}

void Serotonin::reduceImpulsivity(float reductionFactor) {
    if (!pImpl->brain) return;
    
    // Reduce impulsive actions by enhancing decision time
    if (auto* planner = pImpl->brain->getPlanner()) {
        // Increase deliberation time for better decisions
        planner->setDecisionDelay(pImpl->impulsivityReduction * 0.1f);
    }
}

void Serotonin::influenceSocialBehavior(float socialFactor) {
    if (!pImpl->brain) return;
    
    // Enhance social behavior by increasing cooperation signals
    if (auto* motorSystem = pImpl->brain->getMotorSystem()) {
        // This is a placeholder for social behavior modulation
        // In a real implementation, would affect action selection
    }
}

} // namespace nlm
